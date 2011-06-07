//import fli.jlibfli.*;

public class JLibFLITest
{
	public static void main(String s[])
	{
		JLibFLI lib = new JLibFLI();
		int Device;

		System.out.println(lib.FLIGetLibVersion());

		Device = lib.FLIOpen("flipro0", 0x100 | 0x02);

		if (Device != JLibFLI.FLI_INVALID_DEVICE)
		{
			int [] CCD;
			int [] Visible;

			System.out.println("Model: " + lib.FLIGetModel(Device));
			System.out.println("HW Rev: " + lib.FLIGetHWRevision(Device) + " FW Rev: " +
				lib.FLIGetFWRevision(Device));

			CCD = lib.FLIGetArrayArea(Device);
			Visible = lib.FLIGetVisibleArea(Device);

			System.out.println("CCD Area: (" + CCD[0] + "," + CCD[1] + ") to (" + CCD[2]+ "," + CCD[3] + ")");
			System.out.println("Visible Area: (" + Visible[0] + "," + Visible[1] + ") to (" + Visible[2]+ "," + Visible[3] + ")");

			int width, height;
			int hbin = 1;
			int vbin = 1;

			width = (Visible[2] - Visible[0]) / hbin;
			height = (Visible[3] - Visible[1]) / vbin;

			lib.FLISetImageArea(Device, Visible[0], Visible[1],
				Visible[0] + width, Visible[1] + height);

			lib.FLISetTemperature(Device, 5);

			System.out.println("Temperature: " + lib.FLIGetTemperature(Device, 0));

			lib.FLISetHBin(Device, hbin);
			lib.FLISetVBin(Device, vbin);
			lib.FLISetExposureTime(Device, 1000);

			lib.FLISetFrameType(Device, lib.FLI_FRAME_TYPE_DARK);

			lib.FLIExposeFrame(Device);

			int Status;

			do {
				Status = lib.FLIGetExposureStatus(Device);
				System.out.println(Status + " milliseconds left.");

				if (Status > 200) Status = 200;

				try
				{
					Thread.sleep(Status);
				} catch (InterruptedException e)
				{
					e.printStackTrace();
				}
			} while (Status != 0);

			int y;
			short [][] row = new short[height][width];

			for (y = 0; y < height; y++)
			{
				lib.FLIGrabRow(Device, width, row[y]);

				System.out.println(row[y][0]);
			}


		}

		lib.FLIClose(Device);
	}
}