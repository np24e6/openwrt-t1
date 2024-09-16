const process = require('process')
if (Number(process.versions.node.split('.')[0]) < 16) {
  console.log('This script needs node >=v16')
  process.exit(1)
}
const generate = require('./src/generator')
const createServer = require('./src/server')
const { validateChangelogFiles, validateDatabase } = require('./src/validator')
const dataGetters = require('./src/dataGetters')
const fs = require('fs')

process.chdir(__dirname)

if (process.argv.length === 2 || process.argv[2] === 'create') {
  // If database is borked WebUI can be broken
  console.log('Starting server...')
  validateDatabase()
  createServer(25478)
} else if (process.argv[2] === 'dev') {
  createServer(25478, true)
} else if (process.argv[2] === 'validate') {
  validateDatabase()
  validateChangelogFiles()
  console.log('Everything is valid')
  process.exit()
} else if (process.argv[2] === 'generate') {
  globalThis.supressInfo = true
  const type = process.argv[3]
  const device = process.argv[4]
  if (!['wiki', 'git', 'json'].includes(type)) {
    console.log('First parameter should be [wiki|git|json]')
    process.exit(1)
  }
  validateDatabase()
  validateChangelogFiles()
  const devices = dataGetters.getDevices(true)
  if (device && !devices.includes(device)) {
    console.log(
      `Second parameter should be not defined or device:  ${devices
        .sort((a, b) => a.localeCompare(b, undefined, { numeric: true }))
        .join(',')}`
    )
    process.exit(1)
  }
  console.log(generate[type](device))
  process.exit()
} else if (process.argv[2] === 'ci') {
  validateChangelogFiles()
  const folder = '../../wiki_changelogs/'
  const globalChangelog = '../../tmp_changelog'
  fs.rmSync(folder, { recursive: true, force: true })
  fs.mkdirSync(folder)
  dataGetters.getDevices().forEach((device) => {
    const txt = generate.wiki(device.name)
    fs.writeFileSync(`${folder}${device.name}`, txt, { flag: 'w' })
  })
  const globalText = generate.wiki(undefined)
  fs.writeFileSync(`${folder}global`, globalText, { flag: 'w' })
  const gitText = generate.git(undefined)
  fs.writeFileSync(`${globalChangelog}`, gitText, { flag: 'w' })
  process.exit()
} else {
  console.log('./create_changelog.sh [command]')
  console.log('Commands:')
  console.log('  create - starts server and WebUI to create or modify changelog [default]')
  console.log('  validate - validates all changelogs')
  console.log('  generate [wiki|git|json] [device] - generates changelog. By default generates for all devices')
  console.log('  ci - generates all changelog to all devices and global changelog for git')
  console.log('  dev - starts server and WebUI in develop mode (effects vue import)')
}
