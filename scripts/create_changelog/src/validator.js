const dataGetters = require('./dataGetters')

module.exports = { validateChangelogFiles, validateChangelogFile, validateDatabase }

function validateChangelogFiles() {
  /** @type {string[]} */
  const errors = []
  const validFiles = []
  const fileErrors = []
  dataGetters.getChangelogs(true).forEach((file, _, arr) => {
    const idMatch = file.match(/(\d+-).+/)
    if (!file.endsWith('.json'))
      fileErrors.push(`"changelog file "${file}" is not .json file. You Probably used old format use new one`)
    else if (!idMatch)
      fileErrors.push(
        `changelog file "${file}" does not start with case id. You probably created changelog from develop or release - rename file to branch name`
      )
    else if (arr.some((other) => other !== file && other.startsWith(idMatch[1])))
      fileErrors.push(`changelog file "${file}" has dublicate case id in name`)
    else validFiles.push(file)
  })
  errors.push(...fileErrors)
  const changelogIds = validFiles.map((file) => file.split('.')[0])
  changelogIds.forEach((id) => {
    const changelogErrors = validateChangelogFile(dataGetters.getChangelog(id, true))
    if (changelogErrors.length > 0) errors.push(`${id}\n  ${changelogErrors.join('\n  ')}`)
  })
  if (errors.length > 0) {
    console.log(errors.join('\n'))
    process.exit(1)
  }
}

function validateDatabase() {
  function printError(name, errors) {
    if (errors.length > 0) {
      console.log(`There is errors in "${name}" database`)
      console.log(errors.join('\n'))
      process.exit(1)
    }
  }
  const devices = dataGetters.getDevices(true)
  /** @type {Record<string, any[]>} */
  const databases = {
    topics: dataGetters.getTopics(),
    families: dataGetters.getFamilies(),
  }
  const validations = {
    topics: (topic) =>
      validateObject(topic, {
        name: /^[A-Z0-9].*$/,
        category: dataGetters.getCategories(),
        depends: [...dataGetters.getDepends(true, true), undefined],
      }),
    families: (family) =>
      validateObject(family, {
        name: /.*/,
        devices,
      }),
  }
  Object.entries(databases).forEach(([name, database]) => {
    if (!Array.isArray(database)) printError(name, ['database is not array'])
    const errors = database.reduce(
      (errors, item, index) => [
        ...errors,
        ...validations[name](item).map((error) => `- entry #${index + 1} > ${error}`),
      ],
      []
    )
    printError(name, errors)
  })
  const devicesInFamily = databases.families.filter((e) => e.name !== 'Router').flatMap((e) => e.devices)
  const deviceNotInFamily = devices.filter((device) => !devicesInFamily.includes(device))
  printError(
    'families',
    deviceNotInFamily.map((e) => `Device ${e} is not found in any family`)
  )
}

function validateChangelogFile(changelogTxt, caseIdOptions, disableFormatingCheck = false) {
  let changelogs
  try {
    changelogs = JSON.parse(changelogTxt)
  } catch (e) {
    return [e.message]
  }
  if (!Array.isArray(changelogs)) return ['changelog file must be array']
  const changelogErrors = changelogs.flatMap((changelog, index) => {
    const changlogErrors = validateChangelog(changelog, caseIdOptions)
    return changlogErrors.map((error) => `- entry #${index + 1} > ${error}`)
  })
  if (disableFormatingCheck) return changelogErrors
  if (!changelogTxt.endsWith('\n')) changelogErrors.push('- file should end with new line')
  return changelogErrors
}

/**
 * @param {*} object
 * @param {string} key
 * @param {string[]} validOptions
 * @returns {string[]}
 */
function validateOption(object, key, validOptions) {
  const validate = (value) => {
    // No options means that prevous values are bad like non-exicsting category will not have topics
    if (validOptions.length === 0) return []
    if (validOptions.some((option) => JSON.stringify(option) === JSON.stringify(value))) return []
    return [`${key}: ${JSON.stringify(value)} does not exist as a option`]
  }
  if (Array.isArray(object[key])) return object[key].flatMap((value) => validate(value))
  else return validate(object[key])
}

/**
 * @param {*} object
 * @param {string} value
 * @param {RegExp} regex
 * @returns {string[]}
 */
function validateRegex(object, value, regex) {
  if (typeof object[value] !== 'string') return [`${value}: "${object[value]}" is not a string`]
  if (regex.test(object[value])) return []
  return [`${value}: "${object[value]}" fails regex "${regex.source}"`]
}

function validateChangelog(changelog, caseIdOptions) {
  const changeValidators = {
    'CVE Patches': /^CVE-\d*-\d*$/,
    Updates: /^[^ ]*$/,
    default: /^[a-z].*[^ .]$/,
  }
  const topicValidators = {
    'CVE Patches': [undefined],
    Updates: /^.*$/,
    default: dataGetters
      .getTopics()
      .filter((e) => e.category === changelog.category)
      .map((e) => e.name),
  }
  const validationRules = {
    type: dataGetters.getTypes(),
    category: ['CVE Patches'].includes(changelog.type) ? [undefined] : dataGetters.getCategories(),
    topic: topicValidators[changelog.type] ?? topicValidators.default,
    depends: dataGetters.getDepends(true),
    change: changeValidators[changelog.type] ?? changeValidators.default,
    caseId: caseIdOptions ?? /^\d*$/,
    notImportant: [true, undefined],
  }
  return validateObject(changelog, validationRules)
}

/**
 * @param {*} object
 * @param {Record<string, RegExp | any[]>} rules
 * @param {string} rules
 * @returns {string[]}
 */
function validateObject(object, rules) {
  if (typeof object !== 'object') return [`${JSON.stringify(object)} is not a object`]
  return Object.entries(rules).reduce((errors, [value, rule]) => {
    if (!object[value] && !(Array.isArray(rule) && rule.includes(undefined))) errors.push(`${value}: required`)
    else if (rule instanceof RegExp) errors.push(...validateRegex(object, value, rule))
    else errors.push(...validateOption(object, value, rule))
    return errors
  }, /** @type {string[]} */ ([]))
}
