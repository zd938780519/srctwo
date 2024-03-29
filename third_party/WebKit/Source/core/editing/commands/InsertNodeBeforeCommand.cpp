/*
 * Copyright (C) 2005, 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "core/editing/commands/InsertNodeBeforeCommand.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/editing/EditingUtilities.h"

namespace blink {

InsertNodeBeforeCommand::InsertNodeBeforeCommand(
    Node* insert_child,
    Node* ref_child,
    ShouldAssumeContentIsAlwaysEditable
        should_assume_content_is_always_editable)
    : SimpleEditCommand(ref_child->GetDocument()),
      insert_child_(insert_child),
      ref_child_(ref_child),
      should_assume_content_is_always_editable_(
          should_assume_content_is_always_editable) {
  DCHECK(insert_child_);
  DCHECK(!insert_child_->parentNode()) << insert_child_;
  DCHECK(ref_child_);
  DCHECK(ref_child_->parentNode()) << ref_child_;

  DCHECK(HasEditableStyle(*ref_child_->parentNode()) ||
         !ref_child_->parentNode()->InActiveDocument())
      << ref_child_->parentNode();
}

void InsertNodeBeforeCommand::DoApply(EditingState*) {
  ContainerNode* parent = ref_child_->parentNode();
  GetDocument().UpdateStyleAndLayoutTree();
  if (!parent || (should_assume_content_is_always_editable_ ==
                      kDoNotAssumeContentIsAlwaysEditable &&
                  !HasEditableStyle(*parent)))
    return;
  DCHECK(HasEditableStyle(*parent)) << parent;

  parent->InsertBefore(insert_child_.Get(), ref_child_.Get(),
                       IGNORE_EXCEPTION_FOR_TESTING);
}

void InsertNodeBeforeCommand::DoUnapply() {
  GetDocument().UpdateStyleAndLayoutTree();
  if (!HasEditableStyle(*insert_child_))
    return;

  insert_child_->remove(IGNORE_EXCEPTION_FOR_TESTING);
}

DEFINE_TRACE(InsertNodeBeforeCommand) {
  visitor->Trace(insert_child_);
  visitor->Trace(ref_child_);
  SimpleEditCommand::Trace(visitor);
}

}  // namespace blink
