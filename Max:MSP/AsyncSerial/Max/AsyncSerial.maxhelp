{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 6,
			"minor" : 0,
			"revision" : 8
		}
,
		"rect" : [ 22.0, 70.0, 860.0, 784.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"boxes" : [ 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-7",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 857.0, 103.0, 18.0 ],
					"text" : "input_mode 0 0 1"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-12",
					"linecount" : 14,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 417.0, 1113.0, 353.0, 194.0 ],
					"text" : "output_mode <int>isBinary\n\n@abstract \n  set the output processing mode to either binary or ascii. \n@param mode\n  1 indiactes binary mode, which means that received data will be sent out the left outlet sequentially, one byte at a time, as binary data (as [serial] does). 0 indicates ASCII mode, in which received data will be split up into symbols, based on the location of spaces in the received data, and sent out the left outlet as a list. Numbers will also be encoded as symbols, so [fromsymbol] will have to be used to convert them to numbers, if that is the desired behaviour. This mode is useful with caninical input processing.\n"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-3",
					"linecount" : 4,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 28.5, 1161.0, 350.0, 60.0 ],
					"text" : "ports\n\n@abstract \n  print a list of serial ports to tha Max window"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-15",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 507.0, 1083.0, 91.0, 18.0 ],
					"text" : "output_mode 1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-14",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 402.0, 1083.0, 91.0, 18.0 ],
					"text" : "output_mode 0"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-9",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 15.0, 1143.0, 37.0, 18.0 ],
					"text" : "ports"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 15.0, 1479.0, 61.0, 20.0 ],
					"text" : "print recd"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Courier",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-10",
					"linecount" : 43,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 0.0, 0.0, 505.0, 522.0 ],
					"text" : " *     _                           ____            _       _ \n *    / \\   ___ _   _ _ __   ___  / ___|  ___ _ __(_) __ _| |\n *   / _ \\ / __| | | | '_ \\ / __| \\___ \\ / _ \\ '__| |/ _` | |\n *  / ___ \\\\__ \\ |_| | | | | (__   ___) |  __/ |  | | (_| | |\n * /_/   \\_\\___/\\__, |_| |_|\\___| |____/ \\___|_|  |_|\\__,_|_|\n *              |___/    \n * \n * @version\n *  1.0 beta (November 25th 2012)\n *\n * @discussion\n *  An external object for Max/MSP on Mac OSX that interfaces\n *  with serial devices. This is intended to provide more useful\n *  functionality than the built-in [serial] object. Namely, \n *  [AsyncSerial] listens for incoming data on a separate thread\n *  and spits it out whenever it arrives. It does not need to be\n *  polled. This is similar to how UDPReceive works. Similar care\n *  must be taken when dealing with multithreaded Max-patches.\n *  [AsyncSerial] can also have its input buffer flushed,\n *  which is necessary when sending commands to a device and \n *  awaiting a reply. [AsyncSerial] can also be put in and out\n *  of 'canonical' input mode, in which data is only spit out once\n *  a newline char '\\n' is received, which is useful for parsing\n *  text-based replies. [AsyncSerial] also allows users to set \n *  VMIN and VTIME, as described in the unix manual, sub verbo \n *  'termios':\n *\n *  man termios\n *\n *  [AsyncSerial] cannot be polled using 'bang', because, depending \n *  on VMIN, VTIME and ICANON, that could cause the main thread to\n *  hang.\n *  \n *  [AsyncSerial] can either output received data as a sequence \n *   of bytes, as [serial] does, or as a list of text symbols, which\n *   is useful with canonic input processing.\n *\n * @author\n *  Written by Michael Krzyzaniak at Arizona State \n *  University's School of Arts, Media + Engineering\n *  in Fall of 2012.\n *\n *  mkrzyzan@asu.edu"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-4",
					"linecount" : 49,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 15.0, 1530.0, 711.0, 663.0 ],
					"text" : "From the UNIX manual SV 'termios' \n\n\n  Canonical Mode Input Processing\n     In canonical mode input processing, terminal input is processed in units of lines.  A line is delimited by a newline `\\n'\n     character, an end-of-file (EOF) character, or an end-of-line (EOL) character.  See the Special Characters section for more\n     information on EOF and EOL.  This means that a read request will not return until an entire line has been typed, or a signal\n     has been received.  Also, no matter how many bytes are requested in the read call, at most one line is returned.  It is not,\n     however, necessary to read a whole line at once; any number of bytes, even one, may be requested in a read without losing\n     information.\n\n   Noncanonical Mode Input Processing\n     In noncanonical mode input processing, input bytes are not assembled into lines, and erase and kill processing does not occur.\n     The values of the MIN and TIME members of the c_cc array are used to determine how to process the bytes received.\n\n     MIN represents the minimum number of bytes that should be received when the read function successfully returns.  TIME is a\n     timer of 0.1 second granularity that is used to time out bursty and short term data transmissions.  If MIN is greater than {\n     MAX_INPUT}, the response to the request is undefined.  The four possible values for MIN and TIME and their interactions are\n     described below.\n\n   Case A: MIN > 0, TIME > 0\n     In this case TIME serves as an inter-byte timer and is activated after the first byte is received.  Since it is an inter-byte\n     timer, it is reset after a byte is received.  The interaction between MIN and TIME is as follows:  as soon as one byte is\n     received, the inter-byte timer is started.  If MIN bytes are received before the inter-byte timer expires (remember that the\n     timer is reset upon receipt of each byte), the read is satisfied.  If the timer expires before MIN bytes are received, the\n     characters received to that point are returned to the user.  Note that if TIME expires at least one byte is returned because\n     the timer would not have been enabled unless a byte was received.  In this case (MIN > 0, TIME > 0) the read blocks until the\n     MIN and TIME mechanisms are activated by the receipt of the first byte, or a signal is received.  If data is in the buffer at\n     the time of the read(), the result is as if data had been received immediately after the read().\n\n   Case B: MIN > 0, TIME = 0\n     In this case, since the value of TIME is zero, the timer plays no role and only MIN is significant.  A pending read is not\n     satisfied until MIN bytes are received (i.e., the pending read blocks until MIN bytes are received), or a signal is received.\n     A program that uses this case to read record-based terminal I/O may block indefinitely in the read operation.\n\n   Case C: MIN = 0, TIME > 0\n     In this case, since MIN = 0, TIME no longer represents an inter-byte timer.  It now serves as a read timer that is activated\n     as soon as the read function is processed.  A read is satisfied as soon as a single byte is received or the read timer\n     expires.  Note that in this case if the timer expires, no bytes are returned.  If the timer does not expire, the only way the\n     read can be satisfied is if a byte is received.  In this case the read will not block indefinitely waiting for a byte; if no\n     byte is received within TIME*0.1 seconds after the read is initiated, the read returns a value of zero, having read no data.\n     If data is in the buffer at the time of the read, the timer is started as if data had been received immediately after the\n     read.\n\n  Case D: MIN = 0, TIME = 0\n     The minimum of either the number of bytes requested or the number of bytes currently available is returned without waiting for\n     more bytes to be input.  If no characters are available, read returns a value of zero, having read no data.\n\n\n"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-61",
					"linecount" : 14,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 417.0, 889.0, 350.0, 194.0 ],
					"text" : "input_mode <int>vtime <int>vmin <bool>icanon\n\n@abstract \n  set the input processing mode. see the UNIX manual sub verbo 'termios' for a more detailed description, and more information about how the paramaters interact (or scroll down). The values are 0, 0, 0 by default\n\n@param vtime\n  TIME: the interval before read times-out\n@param vmin\n  MIN: the minimum number of bytes to read at once\n@param icanon\n  CANON: 1 for canonical mode, 0 for non-canonical mode."
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-59",
					"linecount" : 9,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 25.0, 1264.0, 350.0, 127.0 ],
					"text" : "print <anything>values\n\n@abstract \n  send ASCII-encoded data to the serial device. A space (ASCII 32) will be inserted between arguments. '1234' will be sent as [31, 32, 33, 34], because ASCII('1') = 31, ASCII('2') = 32, etc...\n\n@param values\n  any ints, floats, or symbols to be ASCII encoded and sent. "
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-60",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 15.0, 1246.0, 96.0, 18.0 ],
					"text" : "print I love 1234"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-58",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 265.5, 1479.0, 60.0, 20.0 ],
					"text" : "print sent"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-55",
					"linecount" : 10,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 417.0, 708.0, 353.0, 141.0 ],
					"text" : "int\n\n@abstract \n  send binary data to the serial device. [AsyncSerial] sends one byte at a time, so this method is intended to recieve numbers in the range 0 - 255. Larger values will be sent in network byte order (most significant byte first), in as few bytes as are required to encode the entire number. For example, 1234 will be sent as [4, 210], because ((4<<8) & 210) = 1234. Negative values are sent as 2s-compliment equivalents.  "
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-54",
					"linecount" : 8,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 417.0, 558.0, 352.0, 114.0 ],
					"text" : "listen <bool>shouldListen\n\n@abstract \n  Start or stop listening for incoming data. A new object will listen by default.\n\n@param shouldListen\n  0 means stop listening, 1 means start."
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-53",
					"linecount" : 7,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 28.5, 1023.0, 350.0, 100.0 ],
					"text" : "flush\n\n@abstract \n  Flush the serial receive buffer. This deletes data that has been received by the operating system but not yet spit out by the [AsyncSerial] object. This is usefull when sending commands to the serial port and them awaiting a reply."
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-52",
					"linecount" : 11,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 28.5, 828.0, 350.0, 154.0 ],
					"text" : "baud <int>rate\n\n@abstract \n  Set the baud rate for commuication with this device. If it is successfully set, or if there is no device open, this value will be remembered for subsequent calls to \"open\". This method performs no boundry checking, so it is up to you to use a valid value. The default baud is 9600.\n\n@param rate\n  the baud rate in bits per second."
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-51",
					"linecount" : 5,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 28.5, 708.0, 350.0, 74.0 ],
					"text" : "close\n\n@abstract \n  If there is a serial port open, close it and restore its settings so it can be used by another program."
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-50",
					"linecount" : 8,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 28.5, 558.0, 350.0, 114.0 ],
					"text" : "open <symbol>port\n\n@abstract \n  If there is already a serial port open, close it and wait one second. Then attempt to open the specified port.\n\n@param port\n  the path to the serial port, to open."
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-48",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 405.0, 690.0, 37.0, 18.0 ],
					"text" : "1234"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-42",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 402.0, 857.0, 103.0, 18.0 ],
					"text" : "input_mode 0 0 0"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-40",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 477.0, 540.0, 49.0, 18.0 ],
					"text" : "listen 1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-39",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 405.0, 540.0, 49.0, 18.0 ],
					"text" : "listen 0"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-36",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 15.0, 1005.0, 36.0, 18.0 ],
					"text" : "flush"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-34",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 15.0, 690.0, 39.0, 18.0 ],
					"text" : "close"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-32",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 15.0, 540.0, 191.0, 18.0 ],
					"text" : "open /dev/cu.usbserial-A9005cw7"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-30",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 15.0, 810.0, 80.0, 18.0 ],
					"text" : "baud 115200"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "int" ],
					"patching_rect" : [ 15.0, 1449.0, 270.0, 20.0 ],
					"text" : "AsyncSerial /dev/cu.usbserial-A9005cw7 115200"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-58", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 411.5, 1108.0, 389.5, 1108.0, 389.5, 1433.0, 24.5, 1433.0 ],
					"source" : [ "obj-14", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 516.5, 1108.0, 390.5, 1108.0, 390.5, 1433.0, 24.5, 1433.0 ],
					"source" : [ "obj-15", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 24.5, 837.0, 10.5, 837.0, 10.5, 1432.0, 24.5, 1432.0 ],
					"source" : [ "obj-30", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 24.5, 570.0, 10.5, 570.0, 10.5, 1434.0, 24.5, 1434.0 ],
					"source" : [ "obj-32", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 24.5, 716.0, 10.5, 716.0, 10.5, 1435.0, 24.5, 1435.0 ],
					"source" : [ "obj-34", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 24.5, 1031.0, 10.5, 1031.0, 10.5, 1433.0, 24.5, 1433.0 ],
					"source" : [ "obj-36", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 414.5, 566.0, 389.5, 566.0, 389.5, 1434.0, 24.5, 1434.0 ],
					"source" : [ "obj-39", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 486.5, 565.0, 389.5, 565.0, 389.5, 1432.0, 24.5, 1432.0 ],
					"source" : [ "obj-40", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 411.5, 881.0, 390.5, 881.0, 390.5, 1433.0, 24.5, 1433.0 ],
					"source" : [ "obj-42", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 414.5, 715.0, 388.5, 715.0, 388.5, 1433.0, 24.5, 1433.0 ],
					"source" : [ "obj-48", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 24.5, 1273.0, 10.5, 1273.0, 10.5, 1433.0, 24.5, 1433.0 ],
					"source" : [ "obj-60", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 519.5, 880.0, 388.0, 880.0, 388.0, 1433.0, 24.5, 1433.0 ],
					"source" : [ "obj-7", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 24.5, 1170.0, 11.5, 1170.0, 11.5, 1433.0, 24.5, 1433.0 ],
					"source" : [ "obj-9", 0 ]
				}

			}
 ],
		"dependency_cache" : [ 			{
				"name" : "AsyncSerial.mxo",
				"type" : "iLaX"
			}
 ]
	}

}
