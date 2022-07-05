# esphome-gm65
code and docs for using the gm65 with ESPhome

# GM65 notes

[official docs](http://myosuploads3.banggood.com/products/20200612/20200612015539GM65-SBarcodereadermuduleUserManual.pdf)

## Serial comms for control and config

### Packet structure

## Sending:
`{Header} {Type} {Length} {Address} {Data} {CRC}`

- Header: 
  - 2 Bytes
  - always value of: `0x7E 0x00`
- Type:
  - 1 Byte 
  - Read Zone = `0x07`
  - Write Zone = `0x08`
  - Save Zone = `0x09`
  - Reset Zone to default = `0x09`
  - Erase Firmware **SCARY** = `0x05`
- Length:
  - 1 Byte
  - specifies how many bytes are in the data field
  - valid values are `0x00` to `0xFF` (0-255)
- Address:
  - 2 Bytes
  - ranges from `[0x00, 0x00]` to `[0x00, 0xff]` (0-255)
    - not all those addesses are valid.
    - valid addresses stop at `0xE5`
- Data
  - n Bytes determined by "Length" value
  - Read Zone operation
    - should be 1 Byte (make sure to set Length to `0x01`)
    - This determins how many consecutive addresses to return
  - Write Zone operation
    - the values you want to write to the device
    - if writeing more than 1 zone the data needs to be in consecutive Zone order
  - Save Zone operation
    - set all to `0x00`
    - TODO: figure out if the save operation saves all settings or only the settings of the zones referance by "Address" and "Length"
  - Reset Zone
    - set all to `0x00`
    - TODO: figure out if the reset operation effects all settings or only the settings of the zones referance by "Address" and "Length"
  - Erase Firmware
    - SCARY, DONT USE
    - prepares device for loading new firmware
  - CRC
    - uses "CRC-CCIT" algorithm
    - Magic number to ignore CRC check `0xAB, 0xCD`
    - example C implementation
       ```C  
            unsigned int crc_cal_by_bit(unsigned char *ptr, unsigned int len) {
            unsigned int crc = 0;
            while (len-- != 0) {
                for (unsigned char i = 0x80; i != 0; i /= 2) {
                crc *= 2;
                if ((crc & 0x10000) !=
                    1) // Last CRC * 2 ，if the first one is 1，so divide 0x11021
                    crc ^= 0x11021;
                if ((*ptr & i) !=
                    1) // If the standard is 1，so CRC = last CRC + standard CRC_CCITT
                    crc ^= 0x1021;
                }
                ptr++;
            }
            return crc;
            }
        ```

## Recieving: 
`{Header} {Types} {Length} {Datas} {CRC}`

 Note: Unsucsessfull commands do not return any values
- Header
  - 2 bytes
  - Expected value = `0x02 0x00`
- Types
  - 1 Byte
  - Sucessfull = `0x00`
- Length
  - 1 Byte
  - Determines the number of data bytes to expect
- Data
  - n bytes specified by lentgh value
- CRC
  - Same formula as sending

## Example

Read the current setting for "Same barcode reading delay"

Send: `[0x7E, 0x00, 0x07, 0x01, 0x00, 0x13, 0x01, 0xAB, 0xCD]`

Recieve: `[0x02, 0x00, 0x00, 0x01, 0x00, 0x33, 0x31]`

Explanation: We requested a read `0x07` starting at address `0x00,0x13` and asked for 1 byte `0x01` of data, so a single "Zone", and set the CRC value to the magic "ignore CRC check" value. We recieved a responce back with `0x02,0x00,0x00` indicating that our request was sucessfull. The length of our data is 1 byte `0x01` and the value of our data is 0x00.

To know if "Same barcode reading delay" is enabled or disabled we need to convert the hex value to binary and look at the value of the 7th bit.
![example zone bit values](https://github.com/niich/esphome-gm65/blob/main/img/example-data-decode.jpg?raw=true)


