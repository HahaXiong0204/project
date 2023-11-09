<template>
	<view class="wrapper">
		<view class="device-area">
			<view class="device-cart">
				<view class="device-info">
					<view class="device-name">温度</view>
					<image class="device-logo" src="/static/Temp.png"></image>
				</view>
				<view class="device-data">{{Temp}}℃</view>
			</view>
			
			<view class="device-cart">
				<view class="device-info">
					<view class="device-name">湿度</view>
					<image class="device-logo" src="/static/Humi.png"></image>
				</view>
				<view class="device-data">{{Humi}}%</view>
			</view>
			
			<view class="device-cart">
				<view class="device-info">
					<view class="device-name">台灯</view>
					<image class="device-logo" src="/static/Lamp.png"></image>
				</view>
				<switch @change="onLedSwitch" color="#2b9939" :checked="Led"/>
			</view>
			
		</view>
		
	</view>
</template>

<script>
	export default {
		data() {
			return {
				Temp: 0,
				Humi: 0,
				Led: false,
			}
		},
		onShow() {
			let that = this
			this.GetDatapoints()
			setInterval(function(){
				that.GetDatapoints()
				}, 3000);
		},
		
		onLoad() {
			
		},
		methods: {
			GetDatapoints: function(){
				uni.request({
				    url: 'http://api.heclouds.com/devices/976362202/datapoints?', //仅为示例，并非真实接口地址。
				
				    header: {
				        'api-key': '7Dfo3B4N7rjezw1BYb1q=eDDgj8=' //自定义请求头信息
				    },
					method: 'GET',
				    success: (res) => {
				  //       console.log(res.data);
						// console.log(res.data.data.datastreams[1].id, res.data.data.datastreams[1].datapoints[0].value);
						// console.log(res.data.data.datastreams[0].id, res.data.data.datastreams[0].datapoints[0].value);
						this.Temp = res.data.data.datastreams[1].datapoints[0].value;
						this.Humi = res.data.data.datastreams[0].datapoints[0].value;
						this.Led = !res.data.data.datastreams[2].datapoints[0].value;
				    }
				});
			},
			onLedSwitch(event) {
				console.log(event.detail.value);
				let sw = event.detail.value;
				if(sw) {
					uni.request({
					    url: 'http://api.heclouds.com/mqtt?topic=LED_SW', 
					
					    header: {
					        'api-key': 'oUCysEwPan3CAAj09zEEhfZwtww=' //Master-key
					    },
						method: 'POST',
						data: {"LED":1},
					    success: (res) => {
					        console.log("LED on!");
					    }
					});
				}
				else {
					uni.request({
					    url: 'http://api.heclouds.com/mqtt?topic=LED_SW', 
					    header: {
					        'api-key': 'oUCysEwPan3CAAj09zEEhfZwtww=' //Master-key
					    },
						method: 'POST',
						data: {"LED":0},
					    success: (res) => {
					        console.log("LED off!");
					    }
					});
				}
			}

		}
	}
</script>

<style>
	.wrapper {
		padding: 30rpx;
	}

	.device-area {
		display: flex;
		justify-content: space-between;
		flex-wrap: wrap;
	}

	.device-cart {
		width: 320rpx;
		height: 150rpx;
		border-radius: 30rpx;
		margin-top: 30rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		/* background-color: #8f8f94; */
		box-shadow: 0 0 15rpx #ccc;
	}

	.device-info {
		font-size: 20rpx;
		/* background-color: #8f8f94; */
	}
	
	.device-name{
		text-align: center;
		color: #6d6d6d;
	}
	.device-logo{
		width: 70rpx;
		height: 70rpx;
		margin-top: 10rpx;
	}
	.device-data{
		font-size: 50rpx;
		color: #6d6d6d;
		
	}
</style>
