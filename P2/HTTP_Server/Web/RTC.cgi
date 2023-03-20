t <html><head><title>Fecha y Hora</title></head>
i pg_header.inc
t <h2 align=center><br>Fecha y Hora</h2>
t <p><font size="2">Permite visualizar la fecha y hora del sistema</font></p>
t <form action=RTC.cgi method=post name=cgi>
t <input type=hidden value="tc" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>Fecha</td>
c h 1 <td> %s </td></tr>
t <tr><td><img src=pabb.gif>Hora</TD>
c h 2 <td> %s </td></tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=submit name=set value="Update" id="sbm">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.