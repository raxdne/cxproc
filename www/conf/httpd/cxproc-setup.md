
# How to setup cxproc with Apache HTTPD

## Ubuntu

### ufw

## Fedora

    # sudo cp ~/cxproc-build/trunk/x86_64-gnu-linux-debug/etc/apache2/sites-available/cxproc-jquery.conf /etc/httpd/conf.d

    #https://wiki.apache.org/httpd/13PermissionDenied
    # namei -m /home/developer/cxproc-build/trunk/x86_64-gnu-linux-debug/www/html/Documents/index.html
    #sudo chmod +x /home/developer/cxproc-build/trunk/x86_64-gnu-linux-debug/www/html
    #sudo chmod +x /home/developer/cxproc-build/trunk/x86_64-gnu-linux-debug/www
    #sudo chmod +x /home/developer/cxproc-build/...

    # namei -m /home/developer/cxproc-build/trunk/x86_64-gnu-linux-debug/www/cgi-bin/cxproc-cgi

### SELinux

	sudo sestatus
    # set SELINUX=permissive in /etc/selinux/config
    # https://www.appnovation.com/blog/change-apache-port-fedora
    #semanage port -l | grep http
    #semanage port -a -t http_port_t -p tcp 8183

    #sudo firewall-cmd --add-service=http --add-service=https --add-port=8183/tcp --permanent
    #sudo firewall-cmd --permanent --service=http --add-port=8183/tcp
    #sudo firewall-cmd --reload
    #sudo firewall-cmd --get-active-zones
    #sudo firewall-cmd --get-zones
    #sudo systemctl enable httpd.service


