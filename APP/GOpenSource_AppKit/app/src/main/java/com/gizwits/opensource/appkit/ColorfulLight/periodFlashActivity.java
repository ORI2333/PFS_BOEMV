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

public class periodFlashActivity extends GosBaseActivity {
    private Button period_red_led_Button;
    Intent intent;
    private GizWifiDevice device;
    private String period_red_led_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_period_flash);
        setActionBar(true, true, "Colorful_Light_Module");
        initDevice();
        period_led_control_init();
    }
    private String switchStatus (){
        String status = "off";
        if(period_red_led_status.equals("off")) {
            status = "p1s";
        }
        if(period_red_led_status.equals("p1s")){
            status = "p05s";
        }
        if(period_red_led_status.equals("p05s")){
            status = "p025s";
        }
        if(period_red_led_status.equals("p025s")){
            status = "p1s";
        }
        return  status;
    }

    private  void setText() {
        if (period_red_led_status.equals("p1s")){
            period_red_led_Button.setText(R.string.period_1s);
            period_red_led_Button.setBackgroundResource(R.drawable.bt_purple);
        } else if(period_red_led_status.equals("p05s")){
            period_red_led_Button.setText(R.string.period_05s);
            period_red_led_Button.setBackgroundResource(R.drawable.bt_purple);
        } else if (period_red_led_status.equals("p025s")) {
            period_red_led_Button.setText(R.string.period_025);
            period_red_led_Button.setBackgroundResource(R.drawable.bt_purple);
        }
    }
    /* period_control_Module*/
    private void period_led_control_init(){
        period_red_led_Button = (Button) findViewById(R.id.period_1s_bt);
        period_red_led_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "pl_s", status);
                if (code == 0) {
                    period_red_led_status = status;
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