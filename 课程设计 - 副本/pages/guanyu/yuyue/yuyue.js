// pages/guanyu/yuyue/yuyue.js
Page({
  data: {
    appointment: null // 存储预约信息
  },

  onLoad: function () {
    // 接收从预约页传递的实时数据
    const eventChannel = this.getOpenerEventChannel();
    if (eventChannel) {
      eventChannel.on('acceptAppointment', (data) => {
        console.log('收到新预约：', data);
        this.setData({ appointment: data });
      });
    }
    // 加载本地存储数据
    this.loadAppointmentInfo();
  },

  onShow: function () {
    // 每次页面显示时刷新数据
    this.loadAppointmentInfo();
  },

  // 从本地存储加载预约信息
  loadAppointmentInfo() {
    try {
      const appointment = wx.getStorageSync('appointment');
      console.log('读取预约信息：', appointment);
      this.setData({ appointment: appointment || null });
    } catch (e) {
      console.error('读取失败：', e);
      wx.showToast({ title: '获取数据失败', icon: 'none' });
    }
  },

  // 取消预约
  cancelAppointment() {
    wx.showModal({
      title: '确认取消',
      content: '确定要取消当前预约吗？',
      success: (res) => {
        if (res.confirm) {
          try {
            wx.removeStorageSync('appointment');
            this.setData({ appointment: null });
            wx.showToast({ title: '预约已取消', icon: 'success' });
          } catch (e) {
            console.error('取消失败：', e);
            wx.showToast({ title: '操作失败，请重试', icon: 'none' });
          }
        }
      }
    });
  },

  // 下拉刷新
  onPullDownRefresh: function () {
    this.loadAppointmentInfo();
    wx.stopPullDownRefresh();
  }
})