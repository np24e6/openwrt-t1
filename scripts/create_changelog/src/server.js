const http = require('http')
const fs = require('fs')
const { exec } = require('child_process')
const dataGetters = require('./dataGetters')
const { validateChangelogFile } = require('./validator')

module.exports = createServer

/**
 * @param {number} port
 * @param {boolean} [devMode]
 */
function createServer(port, devMode = false) {
  let server
  server = http.createServer((req, res) => {
    if (req.method == 'GET' && req.url === '/') {
      return sendData(res, fs.readFileSync('./src/index.html', { encoding: 'utf8' }), 'text/html')
    } else if (req.method == 'GET' && req.url === '/current-branch') {
      return getCurrentBranch(req, res)
    } else if (req.method == 'GET' && req.url === '/options/depends') {
      return sendData(res, JSON.stringify(dataGetters.getDepends()))
    } else if (req.method == 'GET' && req.url === '/options/topics') {
      return sendData(res, JSON.stringify(dataGetters.getTopics()))
    } else if (req.method == 'POST' && req.url === '/options/topics') {
      return addTopic(req, res)
    } else if (req.method == 'POST' && req.url === '/stop') {
      return stopServer(req, res)
    } else if (req.method == 'GET' && req.url?.startsWith('/changelogs/all')) {
      return sendData(res, JSON.stringify(dataGetters.getChangelogs().flatMap((id) => dataGetters.getChangelog(id))))
    } else if (req.method == 'GET' && req.url?.startsWith('/changelogs/')) {
      return getChangelog(req, res)
    } else if (req.method == 'POST' && req.url?.startsWith('/changelogs/all')) {
      return saveChangelogs(req, res)
    } else if (req.method == 'POST' && req.url?.startsWith('/changelogs/')) {
      return saveChangelog(req, res)
    } else {
      res.writeHead(404)
      res.end('Endpoint does not exist')
    }
  })

  server.listen({ port, host: '127.0.0.1' }, () => {
    console.log('Server is started on:', '\x1b[36m', `http://127.0.0.1:${port}`, '\x1b[0m')
    exec(`xdg-open http://127.0.0.1:${port}${devMode ? '#dev' : ''}`, (error, stdout, stderr) => {
      const errorString = `symbol lookup error: /snap/core20/current/lib/x86_64-linux-gnu/libpthread.so.0: undefined symbol: __libc_pthread_init, version GLIBC_PRIVATE`
      if (stderr.includes(errorString)) {
        console.log('\x1b[31m', 'Failed to open browser because https://stackoverflow.com/a/75956168', '\x1b[0m')
        process.exit(1)
      }
    })
    console.log('Changelog WebUI opened')
  })

  server.on('error', (e) => {
    if (e.code === 'EADDRINUSE') {
      createServer(port + 1, devMode)
    } else throw e
  })
}

/**
 * @param {string | undefined} url
 * @returns {string | undefined}
 */
function parseId(url) {
  if (!url) return undefined
  const parts = url.split('/')
  if (parts.length !== 3) return undefined
  const id = parts[2]
  if (!/^[a-zA-Z0-9\-_]+$/.test(id)) return undefined
  return id
}

/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
 */
function getChangelog(req, res) {
  const id = parseId(req.url)
  if (!id) {
    res.writeHead(400)
    res.end(`malformed ID`)
    return
  }
  const changelog = dataGetters.getChangelog(id, true)
  if (changelog !== null) {
    res.end(changelog)
  } else {
    res.writeHead(404)
    res.end(`Changelog "${id}" not found`)
  }
}

/**
 * @param {http.IncomingMessage} req
 */
function getBody(req) {
  return new Promise((resolve) => {
    var body = ''
    req.on('readable', function () {
      const chunk = req.read()
      if (chunk == null) return
      body += chunk
    })
    req.on('end', () => resolve(body))
  })
}

/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
 */
async function saveChangelog(req, res) {
  const id = parseId(req.url)
  if (!id) {
    res.writeHead(400)
    res.end(`malformed ID`)
    return
  }
  const path = `${dataGetters.changelogDir}/${id}.json`
  const body = await getBody(req)
  const parsedBody = JSON.parse(body)
  const prettyBody = JSON.stringify(parsedBody, null, 2) + '\n'
  const validationErrors = validateChangelogFile(prettyBody)
  if (validationErrors.length > 0) {
    res.writeHead(400)
    res.end(validationErrors.join('\n'))
    return
  }
  if (parsedBody.length > 0) fs.writeFileSync(path, prettyBody, { flag: 'w' })
  else if (fs.existsSync(path)) fs.rmSync(path)
  console.log(`Changelog written: ${id}\n\n${prettyBody}`)
  res.writeHead(200)
  res.end('OK')
}

/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
 */
async function saveChangelogs(req, res) {
  const fullIds = dataGetters.getChangelogs()
  const caseIds = fullIds.map((file) => file.split('-')[0])
  const body = await getBody(req)
  const validationErrors = validateChangelogFile(body, caseIds, true)
  if (validationErrors.length > 0) {
    res.writeHead(400)
    res.end(validationErrors.join('\n'))
    return
  }
  const changelogs = JSON.parse(body)
  fullIds.forEach((fullId) => {
    const id = fullId.split('-')[0]
    const entries = changelogs.filter((changelog) => changelog.caseId === id)
    const path = `${dataGetters.changelogDir}/${fullId}.json`
    if (entries.length > 0) fs.writeFileSync(path, JSON.stringify(entries, undefined, 2) + '\n', { flag: 'w' })
    else fs.rmSync(path)
  })
  res.writeHead(200)
  res.end('OK')
}

/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
 */
function getCurrentBranch(req, res) {
  exec('git rev-parse --abbrev-ref HEAD', (err, stdout, stderr) => {
    if (stdout) {
      res.writeHead(200)
      const changelogs = dataGetters.getChangelogs()
      const id = stdout.match(/(\d+-).+/)?.[1]
      if (!id) {
        const excistingFile = changelogs.find((e) => e.endsWith(stdout))
        if (excistingFile) return res.end(excistingFile)
        return res.end(stdout)
      }
      const excistingFile = changelogs.find((e) => e.startsWith(id))
      return res.end(excistingFile ?? stdout)
    } else {
      res.writeHead(500)
      res.end('Internal Server Error')
    }
  })
}

/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
 */
function stopServer(req, res) {
  res.writeHead(200)
  res.end('OK')
  console.log('Server stopped by WebUI')
  setTimeout(() => process.exit(), 100)
}

/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
 */
async function addTopic(req, res) {
  const topics = fs.readFileSync('./databases/topics.json', { encoding: 'utf8' })
  const body = await getBody(req)
  fs.writeFileSync(
    './databases/topics.json',
    JSON.stringify([JSON.parse(body), ...JSON.parse(topics)], undefined, 2) + '\n'
  )
  res.writeHead(200)
  res.end('OK')
}

/**
 * @param {http.ServerResponse} res
 * @param {string} data
 */
function sendData(res, data, type = 'application/json') {
  res.writeHead(200, { 'Content-Type': type })
  res.end(data)
}
