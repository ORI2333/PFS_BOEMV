package com.gizwits.opensource.appkit.ControlModule;

import com.gizwits.opensource.appkit.DeviceModule.GosDeviceListActivity;
import com.gizwits.opensource.appkit.R;
import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.hisilicion.histreaming.GizWifiDevice;
import com.hisilicion.histreaming.GizWifiSDK;

import android.app.ActionBar;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

public class GosDeviceControlActivity extends GosBaseActivity {

	/** The tv MAC */
	private ImageView m_ligthButton;
	private TextView m_textDesc;

	/** The GizWifiDevice device */
	private GizWifiDevice device;
	private String m_status = "Off";

	/** The ActionBar actionBar */
	ActionBar actionBar;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_gos_device_control);
		initDevice();
		setActionBar(true, true, device.getProductName());
		initView();
	}

	private String switchStatus() {
		String status = "Off";
		if (m_status.equals("Off")) {
			status = "On";
		} else {
			status = "Off";
		}

		return status;
	}

	private void setImage() {
		if (m_status.equals("On")) {
			m_ligthButton.setSelected(true);
		} else {
			m_ligthButton.setSelected(false);
		}
	}

	private void initView() {
		m_ligthButton = (ImageView) findViewById(R.id.btn_light_onoff);
		m_textDesc = (TextView)findViewById(R.id.device_desc);

		if (null != device) {
			m_textDesc.setText(device.alias);
		}

		//m_ligthButton.getBackground().setAlpha(0);
		//监听图片的状态，一开始是开的状态
		m_ligthButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				String status = switchStatus();//监听到灯泡被按下，返回当前的状态。
				//通过GizmosWiFiSDK，把数据发送给终端，如果发送成功，返回为0，修改m_status的值，并且把修改图片的状态，失败返回-2
				int code = GizWifiSDK.sharedInstance().post(device.m_index, "status", status);
				if (code == 0) {
					m_status = status;
					setImage();
//					Toast.makeText(GosDeviceControlActivity.this,  "wgm"+m_status, 100).show();//add by wgm
				} else if (code == -2) {
//					Toast.makeText(GosDeviceControlActivity.this, device.alias + " is Offline", 2000).show();
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
