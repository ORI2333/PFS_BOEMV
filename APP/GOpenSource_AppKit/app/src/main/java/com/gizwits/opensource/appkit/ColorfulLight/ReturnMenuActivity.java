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

public class ReturnMenuActivity extends GosBaseActivity {
    private Button colorful_return_main_menu_Button;
    Intent intent;
    private GizWifiDevice device;
    private String colorful_return_main_menu_status = "off";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_return_menu);
        initDevice();
        colorful_return_main_menu();
        setActionBar(true, true, "Return_Menu_Module");
    }
    private String switchStatus (){
        String status = "off";
        if(colorful_return_main_menu_status.equals("off")) {
            status = "clr";
        }
        return  status;
    }

    private  void setText() {
        if (colorful_return_main_menu_status.equals("clr")) {
            colorful_return_main_menu_Button.setText(R.string.colorful_return_main_menu);
            colorful_return_main_menu_Button.setBackgroundResource(R.drawable.cd_68);
        }
    }
    /* return_main_menu*/
    private void colorful_return_main_menu(){
        colorful_return_main_menu_Button = (Button) findViewById(R.id.bt_colorful_return_main_menu);
        colorful_return_main_menu_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "clr_s", status);
                if (code == 0) {
                    colorful_return_main_menu_status = status;
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