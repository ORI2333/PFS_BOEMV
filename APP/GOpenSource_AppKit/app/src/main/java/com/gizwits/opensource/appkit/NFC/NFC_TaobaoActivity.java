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

public class NFC_TaobaoActivity extends GosBaseActivity {
    private Button nfc_taobao_control_Button;
    Intent intent;
    private GizWifiDevice device;
    private String nfc_taobao_control_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_n_f_c__taobao);
        initDevice();
        nfc_taobao_control_init();
        setActionBar(true, true, "NFC");
    }
    private String switchStatus (){
        String status = "off";
        if(nfc_taobao_control_status.equals("off")) {
            status = "nft";
        }
        return  status;
    }

    private  void setText() {
        if (nfc_taobao_control_status.equals("nft")) {
            nfc_taobao_control_Button.setText(R.string.nfc_taobao);
            nfc_taobao_control_Button.setBackgroundResource(R.drawable.solid);
        }
    }
    /* nfc_taobao_control_Module*/
    private void nfc_taobao_control_init(){
        nfc_taobao_control_Button = (Button) findViewById(R.id.bt_nfc_taobao);
        nfc_taobao_control_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "nft_s", status);
                if (code == 0) {
                    nfc_taobao_control_status = status;
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