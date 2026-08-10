import router from '@system.router';
import pm from '@system.internal.package';
import prompt from '@system.prompt' 

export default {
  data: {
    listdata: [],
    // listdata:[{
    //   icons:'/common/icons/baidu.png',
    //   url: "hap://app/com.xiaomi.vela.testcase1/home/index",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/dida.png',
    //   url: "hap://app/com.xiaomi.vela.testcase1",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/fenghuangfm.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/haolv.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/haoqing.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/huarongdao.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/jisuanqi.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/koudai.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/neteasemusic.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/qqmusic.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/rili.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/tangdong.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/tianqi.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/qqmusic.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },

    // {
    //   icons:'/common/icons/baidu.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/dida.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/fenghuangfm.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/haolv.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/haoqing.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/huarongdao.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/jisuanqi.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/koudai.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/neteasemusic.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/qqmusic.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/rili.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/tangdong.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/tianqi.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // },{
    //   icons:'/common/icons/qqmusic.png',
    //   url: "hap://app/com.application.x4b.album",
    //   name:"百度"
    // }],
    isDelete: false,
  },

  onInit() {
    this.listdata = []
    this.refreshAppList();
  },

  onReady() {

  },

  onDestroy() {
    
  },

  deleteConfirm(url) {
    this.isDelete = true;
  },

  clickItem(item) {
    console.log("item =======>", item);
    // 卸载应用
    if (this.isDelete) {
      this.uninstallApp(item.packageName);
      prompt.showToast({
        message: '卸载成功',
        duration: 2000
      })
      this.isDelete = false;
      return;
    }
    // 打开应用
    try {
      router.push({
        uri: item.url
      })
    } catch (error) {
      console.log(error);
    }
  },

  // installApp() {
  //   console.log(pm.installPackage);
  //   console.log(pm.uninstall);
  //   console.log(pm.getAllPackageInfo());
  //   pm.installpackage({
  //     path:"/tmp/com.vela.demo.rpk",
  //     isForce:true,
  //     onInstallProcess:function(packagename, process) {
  //         console.log(packagename, "install process", process);
  //     },
  //     onInstallResult:function(packagename, code, msg) {
  //         console.log(packagename, "install complete:", code, msg);
  //     }
  //   })
  //   this.refreshAppList();
  // },

  uninstallApp(packageName) {
    console.log('packageName=====>', packageName);
    pm.uninstallPackage({
      packageName:packageName,
      isClearCache:true,
      result:function(packagename, code, msg) {
          console.log(packagename,"unistall:", code, ":" ,msg);
      }
    });
    setTimeout(() => {
      this.refreshAppList();
    }, 0);
  },
  refreshAppList() {
    const packageInfo = pm.getAllPackageInfo();
    this.listdata = packageInfo.filter(item => item.packageName !== 'com.app.vela.launcher').map(item => {
      return ({
        icons: `package://${item.packageName + item.icon}`,
        name: item.name,
        url: `hap://app/${item.packageName}`,
        packageName: item.packageName
      })
    })
  },
  closeDeleteIcon(eve){ 
    if (eve.direction === 'right') {
      this.isDelete = false;
    }
  }
}