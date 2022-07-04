# esphome-gm65
code and docs for using the gm65 with ESPhome

# GM65 notes

[official docs](http://myosuploads3.banggood.com/products/20200612/20200612015539GM65-SBarcodereadermuduleUserManual.pdf)

## Serial comms for control and config

### Packet structure

sending:
{Header} {Types} {Lens} {Address} {Datas} {CRC}
recieving: 
{Header} {Types} {Lens} {Datas} {CRC}

