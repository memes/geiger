#! python
#
# Simple script to read data from my geiger counter port, transform the chars
# (remove LCD control characters), and write to stdout.
#
# Designed to be run with feedgnuplot for live data streaming
#

import sys, serial, optparse

def main():
    parser = optparse.OptionParser(
        usage = "%prog [options] [port]",
        description = "Simple script to read data from geiger counter and write to stdout"
    )
    parser.add_option("-p", "--port",
                      dest = "port",
                      help = "serial device to use, default %default%",
                      default = "/dev/ttyUSB0"
                      )
    parser.add_option("-b", "--baud",
                      dest = "baudrate",
                      help = "baud rate for port, default %default",
                      default = 9600
                      )
    parser.add_option("--parity",
                      dest = "parity",
                      help = "port parity, default %default",
                      default = serial.PARITY_NONE
                      )
    parser.add_option("-s", "--stop",
                      dest = "stopbits",
                      help = "stop bits for port, default %default",
                      default = serial.STOPBITS_ONE
                      )
    parser.add_option("--bits",
                      dest = "bytesize",
                      help = "bits per character for port, default %default",
                      default = serial.EIGHTBITS
                      )
    parser.add_option("-c", "--chars",
                      dest = "chars",
                      help = "number of characters to read, default %default",
                      # Geiger LCD is 2 x (16 chars + 2 control chars)
                      default = 36
                      )
    parser.add_option("-t", "--timeout",
                      dest = "timeout",
                      help = "timeout for port, default %default",
                      default = None
                      )
    parser.add_option("-z", "--reset",
                      dest = "reset",
                      action = "store_true",
                      help = "reset geiger counter before starting, default %default",
                      default = False
                      )
    
    (options, args) = parser.parse_args();

    port = None
    try:
        port = serial.Serial(port = options.port,
                             baudrate = options.baudrate,
                             parity = options.parity,
                             stopbits = options.stopbits,
                             bytesize = options.bytesize,
                             timeout = options.timeout)
                             
        if options.reset:
            port.write('z')

        while True:
            buf = port.read(options.chars).decode('ascii', 'ignore')
            counts = [int(s) for s in buf.split() if s.isdigit()]
            if counts:
                sys.stdout.write("%d\n" % counts[0])
                sys.stdout.flush()
                
    except serial.SerialException as se:
        sys.stderr.write("problem reading from port %s: %s\n" % (options.port, se))

    finally:
        if port:
            port.close()
    
if __name__ == '__main__':
    main()
            
            
