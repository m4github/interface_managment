## Display system interface information, add/delete an interface IP address, and add loopback interface in Linux.

### build and debug

    make

### usage

    ./intf_managment show    

    sudo ./intf_managment interface {INTF} {add/del} {ip/mask}   
    or   
    sudo ./intf_managment interface {INTF} {add/del} {ip} {mask}   

    sudo ./intf_managment interface create loopback {INTF}  
