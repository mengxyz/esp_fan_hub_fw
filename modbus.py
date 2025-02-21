from pymodbus.client import ModbusSerialClient
from pymodbus.exceptions import ModbusIOException
from pymodbus.framer.rtu_framer import ModbusRtuFramer

def scan_modbus_slaves(serial_port='/dev/ttyUSB0', baudrate=9600, start_address=1, end_address=247):
    """
    Scans for Modbus RTU slaves in the given address range.
    
    :param serial_port: Serial port to which the Modbus RTU device is connected (e.g., '/dev/ttyUSB0' or 'COM3')
    :param baudrate: Baud rate for the serial communication (default is 9600)
    :param start_address: Start of the Modbus slave address range (default is 1)
    :param end_address: End of the Modbus slave address range (default is 247)
    """
    # Create a Modbus RTU client with ModbusRtuFramer
    client = ModbusSerialClient(
        framer=ModbusRtuFramer,  # Use Modbus RTU protocol
        port=serial_port,        # Serial port (e.g., '/dev/ttyUSB0' or 'COM3')
        baudrate=baudrate,       # Baud rate (default is 9600)
        parity='N',              # Parity (None)
        stopbits=1,              # Stop bits (1)
        bytesize=8,              # Data bits (8)
        timeout=1                # Timeout in seconds
    )

    # Connect to the Modbus RTU server
    if not client.connect():
        print(f"Failed to connect to Modbus RTU server on {serial_port}")
        return

    print(f"Scanning Modbus RTU slaves on {serial_port}...")
    
    # Iterate over the address range
    for slave_id in range(start_address, end_address + 1):
        try:
            # Try reading holding registers (function code 0x03)
            # You can change this to another function code depending on your needs.
            response = client.read_holding_registers(address=0, count=1, slave=slave_id)

            # Check if the response is valid
            if not response.isError():
                print(f"Slave found at address: {slave_id}")
            else:
                # Handle error responses (optional)
                pass
        except ModbusIOException as e:
            # This exception occurs when no response is received from the slave
            continue
        except Exception as e:
            # Catch any other exceptions (optional)
            print(f"Unexpected error scanning address {slave_id}: {e}")

    # Close the connection
    client.close()


if __name__ == "__main__":
    # Set the serial port and baud rate of your Modbus RTU device here
    SERIAL_PORT = '/dev/cu.usbserial-14720'  # Replace with your serial port (e.g., 'COM3' on Windows)
    BAUD_RATE = 9600              # Common baud rates: 9600, 19200, 38400, 57600, 115200

    # Scan Modbus slaves from address 1 to 247
    scan_modbus_slaves(serial_port=SERIAL_PORT, baudrate=BAUD_RATE)
    # scan_modbus(port="/dev/cu.usbserial-14720")  # Change port as needed
