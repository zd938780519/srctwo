// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "chrome/browser/ui/cocoa/tab_contents/sad_tab_view_cocoa.h"
#include "chrome/browser/ui/sad_tab.h"
#include "content/public/browser/web_contents.h"

namespace {

class SadTabCocoa : public chrome::SadTab {
 public:
  SadTabCocoa(content::WebContents* web_contents, chrome::SadTabKind kind)
      : SadTab(web_contents, kind) {
    NSView* web_contents_view = web_contents->GetNativeView();
    sad_tab_view_ =
        [[SadTabView alloc] initWithFrame:web_contents_view.bounds sadTab:this];
    [web_contents_view addSubview:sad_tab_view_];
    [sad_tab_view_ release];
  }

  ~SadTabCocoa() override { [sad_tab_view_ removeFromSuperview]; }

 private:
  // Owned by web_contents
  SadTabView* sad_tab_view_;
};

}  // namespace

chrome::SadTab* chrome::SadTab::Create(content::WebContents* web_contents,
                                       SadTabKind kind) {
  return new SadTabCocoa(web_contents, kind);
}
