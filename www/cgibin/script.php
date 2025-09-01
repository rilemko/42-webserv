<?php

#while (1);

echo nl2br("<!DOCTYPE html><html lang=\"fr\"><head><meta charset=\"utf-8\"><title>CGI: PHP</title></head>\n");
echo nl2br("<h1>Hello from the PHP CGI script!</h1>");
echo nl2br("</br><h2>GET variables:</h2></br>");
echo nl2br("<pre>");

foreach ($_GET as $key => $val)
{
    printf("%- 20s => %s", $key, $val);
    echo "</br>";
}

echo nl2br("</pre>");
echo nl2br("</br><h2>POST variables:</h2></br>");
echo nl2br("<pre>");

foreach ($_POST as $key => $val)
{
    printf("%- 20s => %s", $key, $val);
    echo "</br>";
}

echo nl2br("</pre>");
echo nl2br("</br><h2>Environment:</h2></br>");
echo nl2br("<pre>");

foreach ($_SERVER as $key => $val)
{
    printf("%- 20s => %s", $key, $val);
    echo "</br>";
}

echo nl2br("</pre>");
echo nl2br("</body></html>");
