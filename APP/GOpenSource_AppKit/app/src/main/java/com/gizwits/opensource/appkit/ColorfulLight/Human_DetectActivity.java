package com.gizwits.opensource.appkit.ColorfulLight;

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

public class Human_DetectActivity extends GosBaseActivity {
    private Button human_detect_led_Button;
    Intent intent;
    private GizWifiDevice device;
    private String human_detect_led_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_human__detect);
        initDevice();
        human_detect_led_control_init();
        setActionBar(true, true, "Human_Detect_Module");
    }

    private String switchStatus (){
        String status = "off";
        if(human_detect_led_status.equals("off")) {
            status = "ht";
        }
        return  status;
    }

    private  void setText() {
        if (human_detect_led_status.equals("ht")){
            human_detect_led_Button.setText(R.string.human_detect_start);
            human_detect_led_Button.setBackgroundResource(R.drawable.flesh);
        }
    }
    /* human_detect_control_Module*/
    private void human_detect_led_control_init(){
        human_detect_led_Button = (Button) findViewById(R.id.bt_human_detect);
        human_detect_led_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "ht_s", status);
                if (code == 0) {
                    human_detect_led_status = status;
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