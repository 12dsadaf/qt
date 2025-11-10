// pages/index/index.js
Page({
  data: {
    arr: [],
    Imgs: [
      {
        url: '/pages/images/swiper/mosquito-prevention1.jpg'
      },
      {
        url: '/pages/images/swiper/chikungunya-facts.jpg'
      },
      {
        url: '/pages/images/swiper/campus-mosquito-control.jpg'
      }
    ],
  },

   yuyue:function(){
    wx.switchTab({
      url: '',
    })
   },

   zixun:function() {
    wx.switchTab({
      url: '/pages/faxian/faxian',
    })
  },

  wode:function() {
    wx.switchTab({
      url: '/pages/guanyu/guanyu',
    })
  },

  1:function(){
    wx.navigateTo({
      url: '/pages/faxian/news1/news1',
    })
  },

  2:function(){
    wx.navigateTo({
      url: '/pages/faxian/news2/news2',
    })
  },

  3:function(){
    wx.navigateTo({
      url: '/pages/faxian/news3/news3',
    })
  },

})