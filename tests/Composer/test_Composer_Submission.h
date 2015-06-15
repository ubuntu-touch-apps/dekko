/* Copyright (C) 2006 - 2014 Jan Kundrát <jkt@flaska.net>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TEST_COMPOSER_SUBMISSION
#define TEST_COMPOSER_SUBMISSION

#include <QTest>
#include "Utils/LibMailboxSync.h"
#include "Composer/Submission.h"
#include "MSA/FakeMSA.h"

/** @short Test e-mail submission */
class ComposerSubmissionTest : public LibMailboxSync
{
    Q_OBJECT
public:
    ComposerSubmissionTest();

private slots:
    void testEmptySubmission();
    void testSimpleSubmission();
    void testSimpleSubmissionWithAppendFailed();
    void testSimpleSubmissionWithAppendNoAppenduid();
    void testSimpleSubmissionWithAppendAppenduid();
    void testSimpleSubmissionReplyingToOk();
    void testSimpleSubmissionReplyingToFailedFlags();
    void testMissingFileAttachmentSmtpSave();
    void testMissingFileAttachmentSmtpNoSave();
    void testMissingFileAttachmentBurlSave();
    void testMissingFileAttachmentBurlNoSave();
    void testMissingFileAttachmentImap();
    void testMissingImapAttachmentSmtpSave();
    void testMissingImapAttachmentSmtpNoSave();
    void testMissingImapAttachmentBurlSave();
    void testMissingImapAttachmentBurlNoSave();
    void testMissingImapAttachmentImap();
    void testBurlSubmission();
    void testCatenateBurlWithoutUrlauth();
    void testFailedMsa();
    void testNoImapContinuation();
    void testReplyingNormal();
    void testReplyingToRemoved();
    void init();
    void cleanup();

private:
    void helperTestSimpleAppend(bool appendOk, bool appendUid, bool shallUpdateReplyingTo, bool replyingToUpdateOk);
    void helperSetupProperHeaders();
    void helperMissingAttachment(bool save, bool burl, bool imap, bool attachingFile);
    void helperAttachImapPart(const uint uid);

    Composer::Submission *m_submission;
    MSA::FakeFactory *m_msaFactory;

    QSignalSpy *sendingSpy;
    QSignalSpy *sentSpy;
    QSignalSpy *requestedSendingSpy;
    QSignalSpy *requestedBurlSendingSpy;

    QSignalSpy *submissionSucceededSpy;
    QSignalSpy *submissionFailedSpy;
};

#endif
