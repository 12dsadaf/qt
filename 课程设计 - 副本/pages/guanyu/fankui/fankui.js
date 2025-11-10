function Show(message)
{
  this.message=message;
}
Page({
  data:{
    flag:true,
    flag2:false
  },
  messageInput:function(e){
    this.message=e.detail.value
  },
  showMessage:function(e){
    var p=new Show(this.message)
    this.setData({
      flag:false,
      Show:p,
      flag2:true
    })
    wx.showToast({
      title: '提交成功！',
      icon:'success',
      duration:2000
    })
  },  
})