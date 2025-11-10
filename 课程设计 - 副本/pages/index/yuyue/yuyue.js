Page({
  data: {
    customIndex: [0, 0, 0],
    onlyArray: [
      [],
      [],
      []
    ],
    customArray: [{
        name: '校医院门诊',
        dept: [{
            name: '周一',
            product: [{ name: '8:00' }, { name: '10:00' }, { name: '14:00' }, { name: '16:00' }]
          },
          { name: '周二', product: [{ name: '8:00' }, { name: '10:00' }, { name: '14:00' }, { name: '16:00' }] },
          { name: '周三', product: [{ name: '8:00' }, { name: '10:00' }, { name: '14:00' }, { name: '16:00' }] }
        ]
      },
      {
        name: '南区医疗点',
        dept: [{
            name: '周一',
            product: [{ name: '8:00' }, { name: '10:00' }, { name: '14:00' }]
          },
          { name: '周二', product: [{ name: '8:00' }, { name: '10:00' }, { name: '14:00' }] }
        ]
      },
    ],
  },

  formSubmit: function (e) {
    const name = e.detail.value.name.trim();
    const idCard = e.detail.value.id.trim();
    // 获取选择的地点、日期、时间
    const location = this.data.onlyArray[0][this.data.customIndex[0]];
    const day = this.data.onlyArray[1][this.data.customIndex[1]];
    const time = this.data.onlyArray[2][this.data.customIndex[2]];

    // 1. 姓名验证（2-5个汉字）
    if (!name) {
      wx.showToast({ title: '请输入姓名', icon: 'none' });
      return;
    }
    const nameReg = /^[\u4e00-\u9fa5]{2,5}$/;
    if (!nameReg.test(name)) {
      wx.showToast({ title: '请输入2-5个汉字的姓名', icon: 'none' });
      return;
    }

    // 2. 身份证号验证（18位）
    if (!idCard) {
      wx.showToast({ title: '请输入身份证号', icon: 'none' });
      return;
    }
    const idReg = /^\d{17}[\dXx]$/;
    if (!idReg.test(idCard)) {
      wx.showToast({ title: '请输入有效的18位身份证号', icon: 'none' });
      return;
    }

    // 3. 预约时间验证（不能选择当天之前的日期）
    const today = new Date().getDay() || 7; // 转换为周一为1，周日为7
    const dayMap = { '周一': 1, '周二': 2, '周三': 3, '周四': 4, '周五': 5, '周六': 6, '周日': 7 };
    const selectedDay = dayMap[day];
    
    if (selectedDay < today) {
      wx.showToast({ title: '不能选择已过期的日期', icon: 'none' });
      return;
    }

    // 构建预约信息
    const appointmentInfo = {
      id: Date.now(),
      name,
      idCard,
      location,
      day,
      time,
      createTime: new Date().toLocaleString()
    };

    wx.showModal({
      title: '确认疫苗预约',
      content: `${name}同学，你的预约信息已确认：\n身份证：${idCard}\n地点：${location}\n时间：${day} ${time}\n请按时前往！`,
      success: (res) => {
        if (res.confirm) {
          try {
            // 检查是否已有预约
            const existing = wx.getStorageSync('appointment');
            if (existing) {
              wx.showModal({
                title: '提示',
                content: '您已有预约记录，是否覆盖？',
                success: (overwrite) => {
                  if (overwrite.confirm) {
                    wx.setStorageSync('appointment', appointmentInfo);
                    this.navigateToMyAppointment(appointmentInfo);
                  }
                }
              });
            } else {
              wx.setStorageSync('appointment', appointmentInfo);
              this.navigateToMyAppointment(appointmentInfo);
            }
          } catch (e) {
            console.error('保存失败：', e);
            wx.showToast({ title: '保存失败，请重试', icon: 'none' });
          }
        }
      }
    })
  },

  // 跳转到我的预约页面
  navigateToMyAppointment(info) {
    wx.navigateTo({
      url: '/pages/guanyu/yuyue/yuyue',
      success: (res) => {
        res.eventChannel.emit('acceptAppointment', info);
      }
    });
  },
  
  onLoad: function () {
    const data = {
      customArray: this.data.customArray,
      customIndex: this.data.customIndex,
      onlyArray: this.data.onlyArray,
    };
    // 初始化选择器数据
    data.onlyArray[0] = data.customArray.map(item => item.name);
    data.onlyArray[1] = data.customArray[0].dept.map(item => item.name);
    data.onlyArray[2] = data.customArray[0].dept[0].product.map(item => item.name);
    this.setData(data);
  },
  
  bindCustomPickerChange: function (e) {
    this.setData({ customIndex: e.detail.value });
  },

  bindCustomPickerColumnChange: function (e) {
    const customIndex = [...this.data.customIndex];
    customIndex[e.detail.column] = e.detail.value;
    
    // 根据选择更新下一级选择器
    const { customArray } = this.data;
    const onlyArray = [...this.data.onlyArray.map(arr => [...arr])];
    
    if (e.detail.column === 0) {
      // 切换地点时重置日期和时间
      customIndex[1] = 0;
      customIndex[2] = 0;
      onlyArray[1] = customArray[customIndex[0]].dept.map(item => item.name);
      onlyArray[2] = customArray[customIndex[0]].dept[0].product.map(item => item.name);
    } else if (e.detail.column === 1) {
      // 切换日期时重置时间
      customIndex[2] = 0;
      onlyArray[2] = customArray[customIndex[0]].dept[customIndex[1]].product.map(item => item.name);
    }
    
    this.setData({ onlyArray, customIndex });
  }
})