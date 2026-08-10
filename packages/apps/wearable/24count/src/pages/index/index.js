import router from "@system.router"
export default {
  goLevel(type) {
    router.replace({
      uri: "/pages/game",
      params: {
        typeLevel:type
      }
    })
  }
}