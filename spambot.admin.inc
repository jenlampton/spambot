<?php

function spambot_settings_form($form_state) {
  $form = array();

  $form['criteria'] = array(
    '#type' => 'fieldset',
    '#title' => t('Spammer criteria'),
    '#description' => t('How to decide whether a user or user registration is a spammer?'),
    '#collapsible' => TRUE,
  );
  $form['criteria']['spambot_criteria_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check email'),
    '#description' => t('If the email address for a user or user registration is known to www.stopforumspam.com, then it is treated as a spammer.'),
    '#default_value' => variable_get('spambot_criteria_email', TRUE),
  );
  $form['criteria']['spambot_criteria_username'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check username'),
    '#description' => t('If the username is known to www.stopforumspam.com, then it is treated as a spammer. Be careful about enabling this option as you may accidentally block genuine users who happen to choose the same username as a known spammer.'),
    '#default_value' => variable_get('spambot_criteria_username', FALSE),
  );
  $form['criteria']['spambot_criteria_ip'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check IP address'),
    '#description' => t('If the IP address is known to www.stopforumspam.com, then it is treated as a spammer. Be careful about enabling this option as IP addresses can change, and you may accidentally block genuine users.'),
    '#default_value' => variable_get('spambot_criteria_ip', FALSE),
  );

  $form['existing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Scan existing accounts'),
    '#description' => t('This module can also check existing user accounts to see if they are known spammers. It works by checking user accounts with increasing uid\'s ie. user id 2, 3, 4 etc during cron. Any existing users found to be known spammers are logged. Blocked user accounts are not checked.'),
    '#collapsible' => TRUE,
  );
  $form['existing']['spambot_cron_user_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of user accounts to check per cron'),
    '#description' => t('Enter the number of user accounts to check for each cron. If you do not want to check existing user accounts, leave this as 0.<br />Be careful not to make this value too large, as it will slow your cron execution down and may cause your site to query www.stopforumspam.com more times than allowed each day.'),
    '#size' => 10,
    '#default_value' => variable_get('spambot_cron_user_limit', 0),
  );
  $form['existing']['spambot_spam_account_action'] = array(
    '#type' => 'select',
    '#title' => t('Action to take'),
    '#description' => t('Please select what action to take for user accounts which are found to be spammers.'),
    '#options' => array(
      SPAMBOT_ACTION_NONE => t('None, just log it.'),
      SPAMBOT_ACTION_BLOCK => t('Block user account'),
      SPAMBOT_ACTION_DELETE => t('Delete user account'),
    ),
    '#default_value' => variable_get('spambot_spam_account_action', SPAMBOT_ACTION_NONE),
  );

  // Show scan status
  $last_uid = variable_get('spambot_last_checked_uid', 0);
  $suffix = '';
  if ($last_uid) {
    $num_checked = db_result(db_query("SELECT COUNT(uid) FROM {users} WHERE uid > 1 AND uid <= %d", $last_uid));
    $num_left = db_result(db_query("SELECT COUNT(uid) FROM {users} WHERE uid > 1 AND uid > %d", $last_uid));
    $last_uid = db_result(db_query("SELECT uid FROM {users} WHERE uid > 1 AND uid <= %d ORDER BY uid DESC LIMIT 1", $last_uid));   

    $account = user_load($last_uid);
    $suffix = '<br />' . t('The last checked user account is: !account (uid @uid)', array('!account' => l(check_plain($account->name), 'user/' . $account->uid), '@uid' => $account->uid));
  }
  else {
    $num_checked = 0;
    $num_left = db_result(db_query("SELECT COUNT(uid) FROM {users} WHERE uid > 1"));
  }

  $text = t('Accounts checked: @checked, Accounts remaining: @remaining', array('@checked' => $num_checked, '@remaining' => $num_left));
  $form['existing']['message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Scan status'),
    '#description' => $text . $suffix,
  );

  $form['existing']['spambot_last_checked_uid'] = array(
    '#type' => 'textfield',
    '#title' => t('Continue scanning after this user id'),
    '#size' => 10,
    '#description' => t('Scanning of existing user accounts has progressed to, and including, user id @uid and will continue by scanning accounts after user id @uid. If you wish to change where the scan continues scanning from, enter a different user id here. If you wish to scan all users again, enter a value of 0.', array('@uid' => $last_uid)),
    '#default_value' => $last_uid,
  );

  $form['spambot_sfs_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('www.stopforumspam.com API key'),
    '#description' => 'If you wish to report spammers to Stop Forum Spam, you need to register for an API key at the <a href="http://www.stopforumspam.com">Stop Forum Spam</a> website.',
    '#default_value' => variable_get('spambot_sfs_api_key', FALSE),
  );

  $form['spambot_blocked_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Blocked message'),
    '#default_value' => variable_get('spambot_blocked_message', t('Your email address or username is blacklisted.')),
    '#description' => t('Message to display when user registration is blocked.'),
  );
  return system_settings_form($form);
}
