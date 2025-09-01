<?php

echo nl2br("<pre>");

foreach ($_FILES as $file)
{
    printf("%- 20s => %s", $file["name"], $file["type"]);
    echo "</br>";
}

echo nl2br("</pre>");
