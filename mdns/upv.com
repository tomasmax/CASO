#
# Configuration file for Name Server
#
# own level
upv.com
# servidores registrados
pop.upv.com 158.42.249.29:995
mail.upv.com 158.42.249.29:995
smtp.upv.com 158.42.249.50:465
www.upv.com 158.42.4.23:80
# direcciones de servidores inferiores
dns.upv.com 158.42.249.8:53
# direcciones de servidores superiores
com 127.0.0.1:4444
