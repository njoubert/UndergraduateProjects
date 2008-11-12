<?php
  /**
   * $Id: email.php,v 1.2 2007/07/26 23:46:24 mbasteaf Exp $
   *
   * This Script takes the following params as POST arguments.
   *
   * share - the share ID
   * to - a username to send email to @yahoo-inc.com
   * from - a username to send email from @yahoo-inc.com
   * cc - if set, message will be CCed to the owner
   * jpeg - a Base64 Encoded JPG
   * comment - a note to attach
   * personal - if set, message will be marked private
   * title - the talk's title
   * author - the talk's author
   * duration - duration of the clip
   * durationTotal - total duration of the source media
   * tags - a CSV list of tags
   */
require_once("util/htmlMimeMail5.php");

$to = $_POST['email'];
$subject = $_POST['subject'];
$message = $_POST['message'];

// from: http://www.phpguru.org/static/htmlMimeMail5.html
$mail = new htmlMimeMail5();

// Get this from the cookie?
$mail->setFrom(yck_cookie_get_login() . "@yahoo.com");

$mail->setSubject("Media Share from Happenings: ".$subject);
$mail->setText($message);

$to = $to;
echo $mail->send(array($to));

?>