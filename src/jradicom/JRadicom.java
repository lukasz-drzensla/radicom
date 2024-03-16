package jradicom;

public class JRadicom {

    public interface RCCallbacks {
        public void gps_error_cb();
        public void rtc_error_cb();
        public void alarm_cb();

        public void read_r_cb(rcfdataupck_t fdata);
    }

    public class rcfdataupck_t {
        public String gpsdata;
        public int day;
        public int month;
        public int year;
        public int hours;
        public int minutes;
        public int seconds;
        public int radiation;
    }

    public class rchdr_t {
        boolean qr = false;
        boolean more = false;
        int fc = 0;
        int ec = 0;
    }

    public enum rcstatus_t {
        RC_OK,
        RC_GEN_ERROR,
        RC_ERROR_PARAM,
        RC_ERR_BAD_FRAME,
        RC_ERR_NULL_CB
    }

    public class RC {
        static final int FC_READ = 0;
        static final int FC_MEM_READ = 1;
        static final int FC_SET_DATE_TIME = 2;
        static final int FC_CALIBRATION = 3;

        static final int EC_OK = 0;
        static final int EC_GPS_ERR = 1;
        static final int EC_RTC_ERR = 2;
        static final int EC_ALARM = 3;

        static final int GPS_DATALEN = 80;
    }

    //Functions for sending queries
    private native int[] q_read();
    public native int[] q_memread();
    public native int[] q_setdt(int day, int month, int year, int hours, int minutes, int seconds);
    public native int[] q_calibrate(int ext0, int ext1, int meas0, int meas1);

    //Functions for sending responses - useful for tests
    public native int[] r_read();

    public int[] rc_q_read() 
    {
        return q_read();
    }

    /* Function for decoding header
     * Returns an array of integers of len JHDR_LEN = 4. Meaning of indexes:
     * 0 - qr
     * 1 - more
     * 2 - fc
     * 3 - ec
     */
    private native int[] read_header(int[] frame);
    private native int[] process_read(int[] frame);
    private rchdr_t rc_read_header (int[] frame)
    {
        rchdr_t hdr = new rchdr_t();
        int[] chdr = read_header(frame);
        if (chdr[0] == 0)
        {
            hdr.qr = false;
        } else {
            hdr.qr = true;
        }
        if (chdr[1] == 0)
        {
            hdr.more = false;
        } else {
            hdr.more = true;
        }
        hdr.fc = chdr[2];
        hdr.ec = chdr[3];
        return hdr;
    }

    private rcfdataupck_t rc_process_read(int[] frame)
    {
        rcfdataupck_t tfulldata = new rcfdataupck_t();

        int[] processed = process_read(frame);

        /* length check */
        if (processed.length < RC.GPS_DATALEN + 6) {
            //some error
        }

        tfulldata.gpsdata = "";
        for (int i = 0; i < RC.GPS_DATALEN; i++)
        {
            tfulldata.gpsdata = tfulldata.gpsdata + "" + (char)processed[i];
        }
        tfulldata.day = processed[RC.GPS_DATALEN];
        tfulldata.month = processed[RC.GPS_DATALEN + 1];
        tfulldata.year = processed[RC.GPS_DATALEN + 2];
        tfulldata.hours = processed[RC.GPS_DATALEN + 3];
        tfulldata.minutes = processed[RC.GPS_DATALEN + 4];
        tfulldata.seconds = processed[RC.GPS_DATALEN + 5];
        tfulldata.radiation = processed[RC.GPS_DATALEN + 6];

        return tfulldata;
    }

    public rcstatus_t decode(int[] frame, RCCallbacks callbacks)
    {
        rchdr_t hdr = rc_read_header(frame);

        if (hdr.qr == true)  /* accept only responses */
        {
            /* error checking */
            switch (hdr.ec)
            {
                case RC.EC_OK:
                break;
                case RC.EC_GPS_ERR:
                callbacks.gps_error_cb();
                break;
                case RC.EC_RTC_ERR:
                callbacks.rtc_error_cb();
                break;
                case RC.EC_ALARM:
                callbacks.alarm_cb();
                break;
            }

            /* function checking */
            switch (hdr.ec)
            {
                case RC.FC_READ:
                callbacks.read_r_cb(rc_process_read(frame));
                break;                
                case RC.FC_MEM_READ:
                break;
                case RC.FC_SET_DATE_TIME: //expecting only ACK
                break;
                case RC.FC_CALIBRATION: //expecting only ACK
                break;
            }
        }
        
        return rcstatus_t.RC_OK;
    }

    static {
        System.loadLibrary("CRadicom");
    }
}