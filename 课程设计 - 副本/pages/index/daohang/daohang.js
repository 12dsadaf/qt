Page({ 
  data: {
      latitude: 31.313052,
      longitude: 121.387923,
      currentLocation: '', // 当前位置文本信息
      markers: [{
          id: 1,
          latitude: 31.313052,
          longitude: 121.387923,
          iconPath: '/pages/images/mosquito-marker.png',
          label: {
              content: '防蚊点',
              color: '#0000FF',
              bgColor: '#FFFF00',
              fontSize: 20
          }
      }, {
          id: 2, // 补充id字段
          latitude: 31.312052,
          longitude: 121.388923,
          iconPath: '/pages/images/mosquito-marker.png',
          label: {
              content: '消杀点',
              color: '#0000FF',
              bgColor: '#FFFF00',
              fontSize: 20
          }
      }]
  },

  // 选择位置并显示
  chooseLocation: function () {
      wx.chooseLocation({
          success: (res) => {
              console.log('选择的位置：', res)
              this.setData({
                  currentLocation: res.name + '(' + res.address + ')',
                  latitude: res.latitude,
                  longitude: res.longitude
              })
              wx.showToast({
                  title: '已获取位置',
                  icon: 'success'
              })
          },
          fail: (err) => {
              console.error('选择位置失败：', err)
              this.handleLocationError(err)
          }
      })
  },

  // 获取当前位置并显示
  openLocation: function () {
      // 先检查位置权限
      wx.getSetting({
          success: (res) => {
              // 如果没有授权，请求授权
              if (!res.authSetting['scope.userLocation']) {
                  wx.authorize({
                      scope: 'scope.userLocation',
                      success: () => {
                          this.getAndShowLocation()
                      },
                      fail: () => {
                          // 授权失败，引导用户去设置页开启
                          wx.showModal({
                              title: '权限不足',
                              content: '需要获取位置权限才能使用此功能，请在设置中开启',
                              success: (modalRes) => {
                                  if (modalRes.confirm) {
                                      wx.openSetting()
                                  }
                              }
                          })
                      }
                  })
              } else {
                  // 已授权，直接获取位置
                  this.getAndShowLocation()
              }
          }
      })
  },

  // 获取并显示位置信息
  getAndShowLocation: function () {
      wx.getLocation({
          type: "gcj02", // 修正坐标系类型（原qcj02为错误）
          success: (res) => {
              console.log('位置信息：', res)
              this.setData({
                  latitude: res.latitude,
                  longitude: res.longitude
              })
              
              // 打开地图显示
              wx.openLocation({
                  latitude: res.latitude,
                  longitude: res.longitude,
                  scale: 18, // 修正缩放级别（原28超出范围）
                  name: '当前位置',
                  address: '获取中...'
              })
              
              // 解析地址信息（需要腾讯地图API key）
              this.reverseGeocoder(res.latitude, res.longitude)
          },
          fail: (err) => {
              console.error('获取位置失败：', err)
              this.handleLocationError(err)
          }
      })
  },

  // 逆地理编码获取详细地址
  reverseGeocoder: function (latitude, longitude) {
      // 这里需要替换为你自己的腾讯地图API key
      const key = '你的腾讯地图API密钥'
      wx.request({
          url: `https://apis.map.qq.com/ws/geocoder/v1/?location=${latitude},${longitude}&key=${key}`,
          success: (res) => {
              if (res.data.status === 0) {
                  this.setData({
                      currentLocation: res.data.result.address
                  })
              } else {
                  this.setData({
                      currentLocation: '地址解析失败'
                  })
              }
          },
          fail: () => {
              this.setData({
                  currentLocation: '获取地址失败'
              })
          }
      })
  },

  // 处理位置相关错误
  handleLocationError: function (err) {
      let errorMsg = '获取位置失败'
      if (err.errMsg.includes('deny')) {
          errorMsg = '请在设置中开启位置权限'
      } else if (err.errMsg.includes('fail:auth denied')) {
          errorMsg = '权限已被拒绝'
      }
      wx.showToast({
          title: errorMsg,
          icon: 'none',
          duration: 2000
      })
  }
})