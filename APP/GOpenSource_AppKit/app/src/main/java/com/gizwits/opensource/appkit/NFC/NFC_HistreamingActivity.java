package com.gizwits.opensource.appkit.NFC;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.R;
import com.hisilicion.histreaming.GizWifiDevice;
import com.hisilicion.histreaming.GizWifiSDK;

public class NFC_HistreamingActivity extends GosBaseActivity {
    private Button nfc_histreaming_control_Button;
    Intent intent;
    private GizWifiDevice device;
    private String nfc_histreaming_control_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_n_f_c__histreaming);
        initDevice();
        nfc_histreaming_control_init();
        setActionBar(true, true, "NFC");
    }
    private String switchStatus (){
        String status = "off";
        if(nfc_histreaming_control_status.equals("off")) {
            status = "nfh";
        }
        return  status;
    }

    private  void setText() {
        if (nfc_histreaming_control_status.equals("nfh")) {
            nfc_histreaming_control_Button.setText(R.string.nfc_histreaming);
            nfc_histreaming_control_Button.setBackgroundResource(R.drawable.bt_yellow);
        }
    }
    /* nfc_histreaming_control_Module*/
    private void nfc_histreaming_control_init(){
        nfc_histreaming_control_Button = (Button) findViewById(R.id.bt_nfc_histreaming);
        nfc_histreaming_control_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "nfh_s", status);
                if (code == 0) {
                    nfc_histreaming_control_status = status;
                    setText();
                } else if (code == -2) {
                }
            }
        });
    }
    private void initDevice() {
        Intent intent = getIntent();
        device = (GizWifiDevice) intent.getParcelableExtra("GizWifiDevice");
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                this.finish();
                break;
        }
        return super.onOptionsItemSelected(item);
    }
}