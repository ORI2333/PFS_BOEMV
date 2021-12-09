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

public class NFC_ReturnActivity extends GosBaseActivity {
    private Button return_main_menu_Button;
    Intent intent;
    private GizWifiDevice device;
    private String return_main_menu_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_n_f_c__return);
        initDevice();
        nfc_return_main_menu();
        setActionBar(true, true, "Return_Menu_Module");
    }
    private String switchStatus (){
        String status = "off";
        if(return_main_menu_status.equals("off")) {
            status = "nfr";
        }
        return  status;
    }

    private  void setText() {
        if (return_main_menu_status.equals("nfr")){
            return_main_menu_Button.setText(R.string.nfc_return_main_menu);
            return_main_menu_Button.setBackgroundResource(R.drawable.cd_68);
        }
    }
    /* return_main_menu*/
    private void nfc_return_main_menu(){
        return_main_menu_Button = (Button) findViewById(R.id.bt_nfc_return_main_menu);
        return_main_menu_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "nfr_s", status);
                if (code == 0) {
                    return_main_menu_status = status;
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