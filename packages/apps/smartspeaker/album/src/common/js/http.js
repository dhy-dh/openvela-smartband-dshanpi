const TIMEOUT = 10e3
const fetch = require('@system.fetch')
const QappRequest = (url, data, config) => {
  let start = Date.now();
  const { method = 'GET', headers = {} } = config

  return new Promise((resolve, reject) => {
    if (!data) {
      data = {}
    }
    console.log("### 🚀 album ### fetch start:", url, data);
    fetch.fetch({
      url,
      method,
      data,
      header: headers,
      responseType: 'json',
      success: (res) => {
        console.log('🚀请求成功time:', res.code, Date.now() - start + 'ms');
        if (res) {
          return resolve(res)
        } else {
          console.log('### 🚀 album ### fetch res no res');
          return reject({
            data: 'no res'
          })
        }
      },
      fail: (data, code) => {
        console.log('🚀请求失败time:', data, code, Date.now() - start + 'ms');
        if (code === 401) {
          return resolve({
            data,
            code
          })
        }
        return reject({
          data,
          code
        })
      }
    })
  })

}

export default {
  get: function (url, params, config = {}) {
    return QappRequest(url, params, {
      method: 'GET',
      ...config
    })
  },
  post: function (url, params, config = {}) {
    return QappRequest(url, params, {
      method: 'POST',
      ...config
    })
  }
}