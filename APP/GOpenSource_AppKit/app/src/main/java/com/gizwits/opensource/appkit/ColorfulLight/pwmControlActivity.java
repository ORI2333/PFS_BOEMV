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

public class pwmControlActivity extends GosBaseActivity {
    private Button pwm_red_led_Button;
    Intent intent;
    private GizWifiDevice device;
    private String pwm_red_led_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pwm_control);
        setActionBar(true, true, "PWM_Control_Module");
        pwm_led_control_init();
        initDevice();
    }
    private String switchStatus (){
        String status = "off";
        if(pwm_red_led_status.equals("off")) {
            status = "red_on";
        }
        if(pwm_red_led_status.equals("red_on")){
            status = "green_on";
        }
        if (pwm_red_led_status.equals("green_on")){
            status = "blue_on";
        }
        if(pwm_red_led_status.equals("blue_on")) {
            status = "purple_on";
        }
        if(pwm_red_led_status.equals("purple_on")) {
            status = "white_on";
        }
        if(pwm_red_led_status.equals("white_on")) {
            status = "red_on";
        }
        return  status;
    }

    private  void setText() {
        if (pwm_red_led_status.equals("red_on")){
            pwm_red_led_Button.setText(R.string.pwm_red);
            pwm_red_led_Button.setBackgroundResource(R.drawable.bt_red);
        } else if(pwm_red_led_status.equals("green_on")){
            pwm_red_led_Button.setText(R.string.pwm_green);
            pwm_red_led_Button.setBackgroundResource(R.drawable.bt_green);
        } else if (pwm_red_led_status.equals("blue_on")) {
            pwm_red_led_Button.setText(R.string.pwm_blue);
            pwm_red_led_Button.setBackgroundResource(R.drawable.bt_blue);
        } else if (pwm_red_led_status.equals("purple_on")) {
            pwm_red_led_Button.setText(R.string.pwm_purple);
            pwm_red_led_Button.setBackgroundResource(R.drawable.bt_purple);
        } else if (pwm_red_led_status.equals("white_on")) {
            pwm_red_led_Button.setText(R.string.pwm_all);
            pwm_red_led_Button.setBackgroundResource(R.drawable.bt_white);
        }
    }
    /* PWM_control_Module*/
    private void pwm_led_control_init(){
        pwm_red_led_Button = (Button) findViewById(R.id.pwm_reg_bt);//红
        pwm_red_led_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "pwm_s", status);
                if (code == 0) {
                    pwm_red_led_status = status;
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