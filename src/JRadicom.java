class JRadicom {

    interface RCCallbacks {
        public void gps_error();
        public void rtc_error();
        public void alarm();

        public void read_r_cb(fraddata fdata);
    }

    enum rcstatus_t {
        RC_OK,
        RC_GEN_ERROR,
        RC_ERROR_PARAM,
        RC_ERR_BAD_FRAME,
        RC_ERR_NULL_CB
    }

    class RC {
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
    
    class fraddata {
        String gpsdata;
        int day;
        int month;
        int year;
        int hours;
        int minutes;
        int seconds;
        int radiation;
    }

    class rchdr_t {
        boolean qr = false;
        boolean more = false;
        int fc = 0;
        int ec = 0;
    }

    //Functions for sending requests
    private native int[] q_read();
    private native int[] q_memread();
    private native int[] q_setdt(int day, int month, int year, int hours, int minutes, int seconds);
    private native int[] q_calibrate(int ext0, int ext1, int meas0, int meas1);

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

    private fraddata rc_process_read(int[] frame)
    {
        fraddata tfulldata = new fraddata();

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
                callbacks.gps_error();
                break;
                case RC.EC_RTC_ERR:
                callbacks.rtc_error();
                break;
                case RC.EC_ALARM:
                callbacks.alarm();
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

    class RadAppCB implements RCCallbacks {
        public void gps_error()
        {

        }
        public void rtc_error()
        {

        }
        public void alarm()
        {
            System.out.println("ALARM!!");
        }

        public void read_r_cb(fraddata fdata)
        {
            System.out.println("Read r cb, radiation = " + Integer.toString(fdata.radiation));
        }
    }

    int count = 666;
    private native void print(int x);

    public static void main(String args[]) {
        JRadicom jradicom = new JRadicom();
        RadAppCB radappcb = jradicom.new RadAppCB();
        jradicom.print(5);
        System.out.println("Stuff got from C: " + Integer.toString(jradicom.count));
        System.out.println("Now get an array from C!");
        int[] array = jradicom.q_read();
        for (int i = 0; i < 100; i++)
        {
            System.out.println(Integer.toString(array[i]));
        }
        jradicom.decode(array, radappcb);
    }
    static {
        System.loadLibrary("CRadicom");
    }
}