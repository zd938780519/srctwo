/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "core/dom/LayoutTreeBuilderTraversal.h"

#include "core/HTMLNames.h"
#include "core/dom/FlatTreeTraversal.h"
#include "core/dom/PseudoElement.h"
#include "core/layout/LayoutObject.h"

namespace blink {

inline static bool HasDisplayContentsStyle(const Node& node) {
  return node.IsElementNode() && ToElement(node).HasDisplayContentsStyle();
}

static bool IsLayoutObjectReparented(const LayoutObject* layout_object) {
  if (!layout_object->GetNode()->IsElementNode())
    return false;
  return ToElement(layout_object->GetNode())->IsInTopLayer();
}

void LayoutTreeBuilderTraversal::ParentDetails::DidTraverseInsertionPoint(
    const V0InsertionPoint* insertion_point) {
  if (!insertion_point_) {
    insertion_point_ = insertion_point;
  }
}

inline static void AssertPseudoElementParent(
    const PseudoElement& pseudo_element) {
  DCHECK(pseudo_element.parentNode());
  DCHECK(pseudo_element.parentNode()->CanParticipateInFlatTree());
}

ContainerNode* LayoutTreeBuilderTraversal::Parent(const Node& node,
                                                  ParentDetails* details) {
  // TODO(hayato): Uncomment this once we can be sure
  // LayoutTreeBuilderTraversal::parent() is used only for a node which is
  // connected.
  // DCHECK(node.isConnected());
  if (node.IsPseudoElement()) {
    AssertPseudoElementParent(ToPseudoElement(node));
    return node.parentNode();
  }
  return FlatTreeTraversal::Parent(node, details);
}

ContainerNode* LayoutTreeBuilderTraversal::LayoutParent(
    const Node& node,
    ParentDetails* details) {
  ContainerNode* parent = LayoutTreeBuilderTraversal::Parent(node, details);

  while (parent && HasDisplayContentsStyle(*parent))
    parent = LayoutTreeBuilderTraversal::Parent(*parent, details);

  return parent;
}

LayoutObject* LayoutTreeBuilderTraversal::ParentLayoutObject(const Node& node) {
  ContainerNode* parent = LayoutTreeBuilderTraversal::LayoutParent(node);
  return parent ? parent->GetLayoutObject() : nullptr;
}

Node* LayoutTreeBuilderTraversal::NextSibling(const Node& node) {
  if (node.IsBeforePseudoElement()) {
    AssertPseudoElementParent(ToPseudoElement(node));
    if (Node* next = FlatTreeTraversal::FirstChild(*node.parentNode()))
      return next;
  } else {
    if (node.IsAfterPseudoElement())
      return nullptr;
    if (Node* next = FlatTreeTraversal::NextSibling(node))
      return next;
  }

  Node* parent = FlatTreeTraversal::Parent(node);
  if (parent && parent->IsElementNode())
    return ToElement(parent)->GetPseudoElement(kPseudoIdAfter);

  return nullptr;
}

Node* LayoutTreeBuilderTraversal::PreviousSibling(const Node& node) {
  if (node.IsAfterPseudoElement()) {
    AssertPseudoElementParent(ToPseudoElement(node));
    if (Node* previous = FlatTreeTraversal::LastChild(*node.parentNode()))
      return previous;
  } else {
    if (node.IsBeforePseudoElement())
      return nullptr;
    if (Node* previous = FlatTreeTraversal::PreviousSibling(node))
      return previous;
  }

  Node* parent = FlatTreeTraversal::Parent(node);
  if (parent && parent->IsElementNode())
    return ToElement(parent)->GetPseudoElement(kPseudoIdBefore);

  return nullptr;
}

static Node* LastChild(const Node& node) {
  return FlatTreeTraversal::LastChild(node);
}

static Node* PseudoAwarePreviousSibling(const Node& node) {
  Node* previous_node = LayoutTreeBuilderTraversal::PreviousSibling(node);
  Node* parent_node = LayoutTreeBuilderTraversal::Parent(node);

  if (parent_node && parent_node->IsElementNode() && !previous_node) {
    if (node.IsAfterPseudoElement()) {
      if (Node* child = LastChild(*parent_node))
        return child;
    }
    if (!node.IsBeforePseudoElement())
      return ToElement(parent_node)->GetPseudoElement(kPseudoIdBefore);
  }
  return previous_node;
}

static Node* PseudoAwareLastChild(const Node& node) {
  if (node.IsElementNode()) {
    const Element& current_element = ToElement(node);
    Node* last = current_element.GetPseudoElement(kPseudoIdAfter);
    if (last)
      return last;

    last = LastChild(current_element);
    if (!last)
      last = current_element.GetPseudoElement(kPseudoIdBefore);
    return last;
  }

  return LastChild(node);
}

Node* LayoutTreeBuilderTraversal::Previous(const Node& node,
                                           const Node* stay_within) {
  if (node == stay_within)
    return nullptr;

  if (Node* previous_node = PseudoAwarePreviousSibling(node)) {
    while (Node* previous_last_child = PseudoAwareLastChild(*previous_node))
      previous_node = previous_last_child;
    return previous_node;
  }
  return Parent(node);
}

Node* LayoutTreeBuilderTraversal::FirstChild(const Node& node) {
  return FlatTreeTraversal::FirstChild(node);
}

static Node* PseudoAwareNextSibling(const Node& node) {
  Node* parent_node = LayoutTreeBuilderTraversal::Parent(node);
  Node* next_node = LayoutTreeBuilderTraversal::NextSibling(node);

  if (parent_node && parent_node->IsElementNode() && !next_node) {
    if (node.IsBeforePseudoElement()) {
      if (Node* child = LayoutTreeBuilderTraversal::FirstChild(*parent_node))
        return child;
    }
    if (!node.IsAfterPseudoElement())
      return ToElement(parent_node)->GetPseudoElement(kPseudoIdAfter);
  }
  return next_node;
}

static Node* PseudoAwareFirstChild(const Node& node) {
  if (node.IsElementNode()) {
    const Element& current_element = ToElement(node);
    Node* first = current_element.GetPseudoElement(kPseudoIdBefore);
    if (first)
      return first;
    first = LayoutTreeBuilderTraversal::FirstChild(current_element);
    if (!first)
      first = current_element.GetPseudoElement(kPseudoIdAfter);
    return first;
  }

  return LayoutTreeBuilderTraversal::FirstChild(node);
}

static Node* NextAncestorSibling(const Node& node, const Node* stay_within) {
  DCHECK(!PseudoAwareNextSibling(node));
  DCHECK_NE(node, stay_within);
  for (Node* parent_node = LayoutTreeBuilderTraversal::Parent(node);
       parent_node;
       parent_node = LayoutTreeBuilderTraversal::Parent(*parent_node)) {
    if (parent_node == stay_within)
      return nullptr;
    if (Node* next_node = PseudoAwareNextSibling(*parent_node))
      return next_node;
  }
  return nullptr;
}

Node* LayoutTreeBuilderTraversal::NextSkippingChildren(
    const Node& node,
    const Node* stay_within) {
  if (node == stay_within)
    return nullptr;
  if (Node* next_node = PseudoAwareNextSibling(node))
    return next_node;
  return NextAncestorSibling(node, stay_within);
}

Node* LayoutTreeBuilderTraversal::Next(const Node& node,
                                       const Node* stay_within) {
  if (Node* child = PseudoAwareFirstChild(node))
    return child;
  return NextSkippingChildren(node, stay_within);
}

static Node* NextLayoutSiblingInternal(Node* node, int32_t& limit) {
  for (Node* sibling = node; sibling && limit-- != 0;
       sibling = LayoutTreeBuilderTraversal::NextSibling(*sibling)) {
    if (!HasDisplayContentsStyle(*sibling))
      return sibling;

    if (Node* inner =
            NextLayoutSiblingInternal(PseudoAwareFirstChild(*sibling), limit))
      return inner;

    if (limit == -1)
      return nullptr;
  }

  return nullptr;
}

Node* LayoutTreeBuilderTraversal::NextLayoutSibling(const Node& node,
                                                    int32_t& limit) {
  DCHECK_NE(limit, -1);
  if (Node* sibling = NextLayoutSiblingInternal(NextSibling(node), limit))
    return sibling;

  Node* parent = LayoutTreeBuilderTraversal::Parent(node);
  while (limit != -1 && parent && HasDisplayContentsStyle(*parent)) {
    if (Node* sibling = NextLayoutSiblingInternal(NextSibling(*parent), limit))
      return sibling;
    parent = LayoutTreeBuilderTraversal::Parent(*parent);
  }

  return nullptr;
}

static Node* PreviousLayoutSiblingInternal(Node* node, int32_t& limit) {
  for (Node* sibling = node; sibling && limit-- != 0;
       sibling = LayoutTreeBuilderTraversal::PreviousSibling(*sibling)) {
    if (!HasDisplayContentsStyle(*sibling))
      return sibling;

    if (Node* inner = PreviousLayoutSiblingInternal(
            PseudoAwareLastChild(*sibling), limit))
      return inner;

    if (limit == -1)
      return nullptr;
  }

  return nullptr;
}

Node* LayoutTreeBuilderTraversal::PreviousLayoutSibling(const Node& node,
                                                        int32_t& limit) {
  DCHECK_NE(limit, -1);
  if (Node* sibling =
          PreviousLayoutSiblingInternal(PreviousSibling(node), limit))
    return sibling;

  Node* parent = LayoutTreeBuilderTraversal::Parent(node);
  while (limit != -1 && parent && HasDisplayContentsStyle(*parent)) {
    if (Node* sibling =
            PreviousLayoutSiblingInternal(PreviousSibling(*parent), limit))
      return sibling;
    parent = LayoutTreeBuilderTraversal::Parent(*parent);
  }

  return nullptr;
}

Node* LayoutTreeBuilderTraversal::FirstLayoutChild(const Node& node) {
  int32_t limit = kTraverseAllSiblings;
  return NextLayoutSiblingInternal(PseudoAwareFirstChild(node), limit);
}

LayoutObject* LayoutTreeBuilderTraversal::NextSiblingLayoutObject(
    const Node& node,
    int32_t limit) {
  DCHECK(limit == kTraverseAllSiblings || limit >= 0) << limit;
  for (Node* sibling = NextLayoutSibling(node, limit); sibling && limit != -1;
       sibling = NextLayoutSibling(*sibling, limit)) {
    LayoutObject* layout_object = sibling->GetLayoutObject();
    if (layout_object && !IsLayoutObjectReparented(layout_object))
      return layout_object;
  }
  return nullptr;
}

LayoutObject* LayoutTreeBuilderTraversal::PreviousSiblingLayoutObject(
    const Node& node,
    int32_t limit) {
  DCHECK(limit == kTraverseAllSiblings || limit >= 0) << limit;
  for (Node* sibling = PreviousLayoutSibling(node, limit);
       sibling && limit != -1;
       sibling = PreviousLayoutSibling(*sibling, limit)) {
    LayoutObject* layout_object = sibling->GetLayoutObject();
    if (layout_object && !IsLayoutObjectReparented(layout_object))
      return layout_object;
  }
  return nullptr;
}

LayoutObject* LayoutTreeBuilderTraversal::NextInTopLayer(
    const Element& element) {
  if (!element.IsInTopLayer())
    return nullptr;
  const HeapVector<Member<Element>>& top_layer_elements =
      element.GetDocument().TopLayerElements();
  size_t position = top_layer_elements.Find(&element);
  DCHECK_NE(position, kNotFound);
  for (size_t i = position + 1; i < top_layer_elements.size(); ++i) {
    if (LayoutObject* layout_object = top_layer_elements[i]->GetLayoutObject())
      return layout_object;
  }
  return nullptr;
}

}  // namespace blink
