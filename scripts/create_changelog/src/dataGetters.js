const fs = require('fs')
const execSync = require('child_process').execSync

let isVuciPrepared = false

async function prepareVuci() {
  if (isVuciPrepared) return
  try {
    if (!globalThis.supressInfo) console.log('Updating vuci feeds. This might take a while...')
    execSync('./scripts/feeds update vuci', { cwd: '../../', stdio: 'ignore' })
  } catch (e) {
    console.log('Failed to update vuci: ' + e)
    process.exit(1)
  }
  isVuciPrepared = true
}

module.exports = {
  changelogDir: '../../.gitlab/changelogs',
  deviceHwDir: '../../feeds/vuci/api-documentation/devices',
  /** @type {null | Record<string, any>} */
  deviceHwCache: null,
  getTypes() {
    return ['New', 'Improvements', 'Fix', 'CVE Patches', 'Updates']
  },
  getCategories() {
    return ['Network', 'Services', 'System']
  },
  getTopics(noDepends = true) {
    const txt = fs.readFileSync('./databases/topics.json', { encoding: 'utf8' })
    return JSON.parse(txt)
  },
  getFamilies(noDevices) {
    const txt = fs.readFileSync('./databases/families.json', { encoding: 'utf8' })
    const parsedData = JSON.parse(txt).map((family) => ({
      type: 'family',
      name: family.name,
      devices: noDevices ? undefined : family.devices,
    }))
    // generating Router family in js as it would require a lot of dublication in json
    parsedData.push({
      type: 'family',
      name: 'Router',
      devices: noDevices
        ? undefined
        : this.getDevices()
            .map((e) => e.name)
            .filter((e) => !e.startsWith('TAP')),
    })
    return parsedData
  },
  getDevices(raw) {
    prepareVuci()
    const exceptions = ['TSW202', 'TSW212', 'RUT206', 'RUT240', 'RUT950', 'RUT955']
    const devices = fs
      .readdirSync(this.deviceHwDir)
      .map((device) => device.split('.')[0].toUpperCase())
      .filter((device) => !exceptions.includes(device))
    if (raw) return devices
    return devices.map((device) => ({ type: 'device', name: device }))
  },
  getDepends(noDevices, allPosible) {
    return [...this.getDevices(), ...this.getFamilies(noDevices), ...this.getHwDepends(allPosible)]
  },
  getChangelog(id, raw) {
    const path = `${this.changelogDir}/${id}.json`
    if (fs.existsSync(path)) {
      const txt = fs.readFileSync(path, { encoding: 'utf8' })
      if (raw) return txt
      return JSON.parse(txt)
    } else {
      return null
    }
  },
  getChangelogs(full) {
    const changelogFiles = fs.readdirSync(this.changelogDir).filter((file) => file !== '.gitkeep')
    if (full) return changelogFiles
    return changelogFiles.map((file) => file.split('.')[0])
  },
  getJsonFile(name, raw) {
    const path = name
    if (fs.existsSync(path)) {
      const txt = fs.readFileSync(path, { encoding: 'utf8' })
      if (raw) return txt
      return JSON.parse(txt)
    } else {
      return null
    }
  },
  getDevicesHw() {
    if (this.deviceHwCache) this.deviceHwCache
    prepareVuci()
    const changelogFiles = fs
      .readdirSync(this.deviceHwDir)
      .map((device) => [device.split('.')[0].toUpperCase(), this.getJsonFile(`${this.deviceHwDir}/${device}`)])
    const res = Object.fromEntries(changelogFiles)
    this.deviceHwCache = res
    return res
  },
  getPosibleHwFlags() {
    return [...new Set(Object.values(this.getDevicesHw()).flatMap((deviceHw) => Object.keys(deviceHw)))]
  },
  getHwDepends(allPosible) {
    if (allPosible)
      return this.getPosibleHwFlags().flatMap((hwFlag) => [
        { type: 'hw', name: hwFlag },
        { type: 'hw', name: hwFlag, value: false },
      ])
    // This is just subset. There is too many to be shown in ui
    return [
      { type: 'hw', name: 'mobile' },
      { type: 'hw', name: 'mobile', value: false },
      { type: 'hw', name: 'wifi' },
      { type: 'hw', name: 'wifi', value: false },
      { type: 'hw', name: 'wifi5Ghz' },
      { type: 'hw', name: 'ios' },
      { type: 'hw', name: 'ios', value: false },
      { type: 'hw', name: 'gps' },
      { type: 'hw', name: 'gps', value: false },
      { type: 'hw', name: 'port_link' },
      { type: 'hw', name: 'port_link', value: false },
      { type: 'hw', name: 'usb' },
      { type: 'hw', name: 'usb', value: false },
      { type: 'hw', name: 'flash16mb' },
      { type: 'hw', name: 'flash16mb', value: false },
      { type: 'hw', name: 'poe' },
    ]
  },
}
