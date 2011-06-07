//package fli.jlibfli;

public class JLibFLI
{
	public static void main(String args[])
	{
		JLibFLI lib = new JLibFLI();
	}

	public static final int FLI_INVALID_DEVICE = (-1);
	public static final int FLIDOMAIN_NONE = (0x00);
	public static final int FLIDOMAIN_PARALLEL_PORT = (0x01);
	public static final int FLIDOMAIN_USB = (0x02);
	public static final int FLIDOMAIN_SERIAL = (0x03);
	public static final int FLIDOMAIN_INET = (0x04);
	public static final int FLIDOMAIN_SERIAL_19200 = (0x05);
	public static final int FLIDOMAIN_SERIAL_1200 = (0x06);
	public static final int FLIDEVICE_NONE = (0x000);
	public static final int FLIDEVICE_CAMERA = (0x100);
	public static final int FLIDEVICE_FILTERWHEEL = (0x200);
	public static final int FLIDEVICE_FOCUSER = (0x300);
	public static final int FLIDEVICE_ENUMERATE_BY_CONNECTION = (0x8000);
	public static final int FLI_FRAME_TYPE_NORMAL = (0);
	public static final int FLI_FRAME_TYPE_DARK = (1);
	public static final int FLI_FRAME_TYPE_FLOOD = (2);
	public static final int FLI_FRAME_TYPE_RBI_FLUSH = (FLI_FRAME_TYPE_FLOOD | FLI_FRAME_TYPE_DARK);
	public static final int FLI_SHUTTER_CLOSE = (0x0000);
	public static final int FLI_SHUTTER_OPEN = (0x0001);
	public static final int FLI_SHUTTER_EXTERNAL_TRIGGER = (0x0002);
	public static final int FLI_SHUTTER_EXTERNAL_TRIGGER_LOW = (0x0002);
	public static final int FLI_SHUTTER_EXTERNAL_TRIGGER_HIGH = (0x0004);
	public static final int FLI_BGFLUSH_STOP = (0x0000);
	public static final int FLI_BGFLUSH_START = (0x0001);
	public static final int FLI_TEMPERATURE_INTERNAL = (0x0000);
	public static final int FLI_TEMPERATURE_EXTERNAL = (0x0001);
	public static final int FLI_TEMPERATURE_CCD = (0x0000);
	public static final int FLI_TEMPERATURE_BASE = (0x0001);
	public static final int FLI_CAMERA_STATUS_UNKNOWN = (0xffffffff);
	public static final int FLI_CAMERA_STATUS_MASK = (0x000000ff);
	public static final int FLI_CAMERA_STATUS_IDLE = (0x00);
	public static final int FLI_CAMERA_STATUS_WAITING_FOR_TRIGGER = (0x01);
	public static final int FLI_CAMERA_STATUS_EXPOSING = (0x02);
	public static final int FLI_CAMERA_STATUS_READING_CCD = (0x03);
	public static final int FLI_CAMERA_DATA_READY = (0x80000000);
	public static final int FLI_FOCUSER_STATUS_UNKNOWN = (0xffffffff);
	public static final int FLI_FOCUSER_STATUS_HOMING = (0x00000004);
	public static final int FLI_FOCUSER_STATUS_MOVING_IN = (0x00000001);
	public static final int FLI_FOCUSER_STATUS_MOVING_OUT = (0x00000002);
	public static final int FLI_FOCUSER_STATUS_MOVING_MASK = (0x00000007);
	public static final int FLI_FOCUSER_STATUS_HOME = (0x00000080);
	public static final int FLI_FOCUSER_STATUS_LEGACY = (0x10000000);
	public static final int FLIDEBUG_NONE = (0x00);
	public static final int FLIDEBUG_INFO = (0x01);
	public static final int FLIDEBUG_WARN = (0x02);
	public static final int FLIDEBUG_FAIL = (0x04);
	public static final int FLIDEBUG_ALL = (FLIDEBUG_INFO | FLIDEBUG_WARN | FLIDEBUG_FAIL);

	public static final int FLI_IO_P0 = (0x01);
	public static final int FLI_IO_P1 = (0x02);
	public static final int FLI_IO_P2 = (0x04);
	public static final int FLI_IO_P3 = (0x08);


	public native String FLIGetLibVersion();
	public native int FLIOpen(String File, int domain);
	public native int FLIClose(int device);
	public native String FLIGetModel(int dev);
	public native int FLIGetHWRevision(int dev);
	public native int FLIGetFWRevision(int dev);
	public native int[] FLIGetArrayArea(int dev);
	public native int[] FLIGetVisibleArea(int dev);
	public native double[] FLIGetPixelSize(int dev);
	public native int FLISetExposureTime(int dev, int exptime);
	public native int FLISetImageArea(int dev, int ul_x, int ul_y, int lr_x, int lr_y);
	public native int FLISetHBin(int dev, int hbin);
	public native int FLISetVBin(int dev, int vbin);
	public native int FLICancelExposure(int dev);
	public native int FLIGetExposureStatus(int dev);
	public native int FLISetTemperature(int dev, double temperature);
	public native double FLIGetTemperature(int dev, int channel);
	public native int FLIExposeFrame(int dev);
	public native int FLIGrabRow(int dev, int width, short[] array);
	public native int FLISetFrameType(int dev, int frameType);

	static
	{
		try
		{
			System.loadLibrary("jlibfli");
		}
		catch (java.lang.UnsatisfiedLinkError e)
		{
			System.out.println("Native library failed to load.");
			System.out.println("Java.library.path is "+ System.getProperty("java.library.path"));
			e.printStackTrace();
		}
	}




}