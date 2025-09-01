<?php

echo nl2br("<pre>");

foreach ($_POST as $key => $val)
{
    printf("%- 20s => %s", $key, $val);
    echo "</br>";
}

echo nl2br("</pre>");
