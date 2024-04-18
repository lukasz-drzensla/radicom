package example;
import jradicom.JRadicom;

/* callbacks implementing application-specific functions */
class RadAppCB implements JRadicom.RCCallbacks {

    public void gps_error_cb()
    {
        System.out.println("GPS ERROR!!");
    }

    public void rtc_error_cb()
    {
        System.out.println("RTC ERROR!!");
    }

    public void alarm_cb()
    {
        System.out.println("ALARM!!");
    }

    public void read_r_cb(JRadicom.rcfdataupck_t fdata)
    {
        String radiation = Integer.toString(fdata.radiation);
        String day = Integer.toString(fdata.day);
        String month = Integer.toString(fdata.month);
        String year = Integer.toString(fdata.year);
        String hours = Integer.toString(fdata.hours);
        String minutes = Integer.toString(fdata.minutes);
        String seconds = Integer.toString(fdata.seconds);
        System.out.printf("Radiation: %s, Day: %s, Month: %s, Year: %s, Hours: %s, Minutes: %s, Seconds: %s, GPS: %s\n", radiation, day,
            month, year, hours, minutes, seconds, fdata.gpsdata);
    }
}

class Main {
    public static void main(String args[]) {
        JRadicom jradicom = new JRadicom();
        RadAppCB radappcb = new RadAppCB();
        int[] frame = jradicom.rc_q_read(); //send query
        //some send function
        //some wait for response
        frame = jradicom.rc_r_read(); //for testing - reply to ourselves

        for (int i = 0; i < 100; i++)
        {
            System.out.print("0x" + Integer.toHexString(frame[i]) + " ");
        }
        System.out.println("");
        jradicom.decode(frame, radappcb); //decode response
    }
}