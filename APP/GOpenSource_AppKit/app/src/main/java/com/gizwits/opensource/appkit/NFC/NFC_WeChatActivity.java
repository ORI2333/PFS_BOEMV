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

public class NFC_WeChatActivity extends GosBaseActivity {
    private Button nfc_control_Button;
    Intent intent;
    private GizWifiDevice device;
    private String nfc_control_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_n_f_c);
        initDevice();
        nfc_wechat_control_init();
        setActionBar(true, true, "NFC");
    }
    private String switchStatus (){
        String status = "off";
        if(nfc_control_status.equals("off")) {
            status = "nfw";
        }
        return  status;
    }

    private  void setText() {
        if (nfc_control_status.equals("nfw")) {
            nfc_control_Button.setText(R.string.nfc_wechat);
            nfc_control_Button.setBackgroundResource(R.drawable.bt_green);
        }
    }
    /* nfc_control_Module*/
    private void nfc_wechat_control_init(){
        nfc_control_Button = (Button) findViewById(R.id.bt_nfc_app);
        nfc_control_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "nfw_s", status);
                if (code == 0) {
                    nfc_control_status = status;
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