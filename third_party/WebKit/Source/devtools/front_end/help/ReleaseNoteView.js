// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

Help.ReleaseNoteView = class extends UI.VBox {
  constructor() {
    super(true);
    this.registerRequiredCSS('help/releaseNote.css');
    var releaseNoteElement = this._createReleaseNoteElement(Help.latestReleaseNote());
    var topSection = this.contentElement.createChild('div', 'release-note-top-section');
    topSection.textContent = Common.UIString(Help.latestReleaseNote().header);
    this.contentElement.appendChild(releaseNoteElement);
  }

  /**
   * @param {!Help.ReleaseNote} releaseNote
   * @return {!Element}
   */
  _createReleaseNoteElement(releaseNote) {
    var hbox = createElementWithClass('div', 'hbox');
    var container = hbox.createChild('div', 'release-note-container');
    var contentContainer = container.createChild('ul');
    for (var highlight of releaseNote.highlights) {
      var listItem = contentContainer.createChild('li');
      var title = UI.createExternalLink(highlight.link, highlight.title + ' ', 'release-note-title');
      title.title = '';
      listItem.appendChild(title);
      var subtitle = UI.createExternalLink(highlight.link, highlight.subtitle + ' ', 'release-note-subtitle');
      subtitle.title = '';
      listItem.appendChild(subtitle);
    }

    var actionContainer = container.createChild('div', 'release-note-action-container');
    actionContainer.appendChild(UI.createTextButton(Common.UIString('Learn more'), event => {
      event.consume(true);
      InspectorFrontendHost.openInNewTab(releaseNote.link);
    }));
    actionContainer.appendChild(UI.createTextButton(Common.UIString('Close'), event => {
      event.consume(true);
      UI.inspectorView.closeDrawerTab(Help.releaseNoteViewId, true);
    }, 'close-release-note'));

    var imageLink = UI.createExternalLink(releaseNote.link, ' ');
    imageLink.classList.add('release-note-image');
    imageLink.title = '';
    hbox.appendChild(imageLink);
    var image = imageLink.createChild('img');
    image.src = 'Images/whatsnew.png';
    return hbox;
  }
};
