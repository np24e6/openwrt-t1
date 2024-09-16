const dataGetters = require('./dataGetters')

module.exports = { git: generateChangelogGit, wiki: generateChangelogWiki, json: generateChangelogJson }

/**
 * Formats changelog using json format
 */
function generateChangelogJson(device) {
  const changelogs = dataGetters.getChangelogs().flatMap((id) => dataGetters.getChangelog(id))
  const filteredChangelogs = filterChangelogs(changelogs, device, false)
  return JSON.stringify(filteredChangelogs, undefined, 2)
}

/**
 * Formats changelog using markdown format for use in git repo
 */
function generateChangelogGit(device) {
  const changelogs = dataGetters.getChangelogs().flatMap((id) => dataGetters.getChangelog(id))
  const filteredChangelogs = filterChangelogs(changelogs, device, false)

  const changesByType = groupBy(filteredChangelogs, (change) => change.type)
  let typeText = Object.entries(changesByType)
    .sort(sortByType)
    .map(([type, changes]) => {
      const changeTexts = changes
        .map(({ topic, change, depends, caseId }) => {
          const parsedDepends = depends
            ?.filter((depend) => depend.type !== 'hw')
            .map((depend) => `${depend.name}${depend.type !== 'device' ? ` ${depend.type}` : ''}`)
          // 'for' only shown in global changelog when change has depends
          const forText = !device && parsedDepends?.length > 0 ? ` for ${parsedDepends.join(', ')}` : ''
          if (type === 'CVE Patches') return `* patched ${change}${forText} (ID: ${caseId})`
          if (type === 'Updates') return `* ${topic}: updated version to  ${change}${forText} (ID: ${caseId})`
          return `* ${topic}: ${change}${forText} (ID: ${caseId})`
        })
        .sort((a, b) => a.localeCompare(b, undefined, { numeric: true }))
      if (changeTexts.length === 0) return ''
      return `### ${type}\n\n${changeTexts.join('\n')}`
    })
  typeText = typeText.filter((e) => e)
  if (typeText.length > 0) return `## Changelog\n\n${typeText.join('\n\n')}\n`
  return 'No changes'
}

/**
 * Formats changelog using wikitext format for use in wiki
 */
function generateChangelogWiki(device) {
  const changelogs = dataGetters.getChangelogs().flatMap((id) => dataGetters.getChangelog(id))
  const filteredChangelogs = filterChangelogs(changelogs, device, true)
  const changedChangelog = modifyChangelog(filteredChangelogs)
  const changesByType = groupBy(changedChangelog, (change) => change.type)
  let typeText = Object.entries(changesByType)
    .sort(sortByType)
    .map(([type, changes]) => {
      let categoryText = []
      // CVE patches does not have categories or topics
      if (type === 'CVE Patches') {
        categoryText = changes
          .sort((a, b) => a.change.localeCompare(b.change, undefined, { numeric: true }))
          .map((change) => `** ${change.change}`)
      } else {
        const changesByCat = groupBy(changes, (change) => change.category)
        categoryText = Object.entries(changesByCat)
          .sort(sortByCategory)
          .map(([category, changes]) => {
            const changeTexts = changes
              .sort((a, b) => a.topic.localeCompare(b.topic))
              // Updates goes last
              .sort((a, b) => {
                const aUpdates = a.oldType === 'Updates' ? 1 : 0
                const bUpdates = b.oldType === 'Updates' ? 1 : 0
                return aUpdates - bUpdates
              })
              .map(({ topic, change, depends }) => {
                const parsedDepends = depends
                  ?.filter((depend) => depend.type !== 'hw')
                  .map((depend) => `${depend.name}${depend.type !== 'device' ? ` ${depend.type}` : ''}`)
                // 'for' only shown in global changelog when change has depends
                const forText = !device && parsedDepends.length > 0 ? ` for ${parsedDepends.join(', ')}` : ''
                return `*** ${topic}: ${change}${forText}`
              })
            if (changeTexts.length === 0) return ''
            return `** <b>${category}</b>\n${changeTexts.join('\n')}`
          })
      }
      categoryText = categoryText.filter((e) => e)
      if (categoryText.length === 0) return ''
      return `* <b>${type}</b>\n${categoryText.join('\n')}`
    })
  typeText = typeText.filter((e) => e)
  if (typeText.length === 0) return 'No changes'
  return typeText.join('\n')
}

/**
 * Filters changelog
 * @param {*[]} changelogs
 * @param {string} device
 * @param {boolean} isClean - is changelog for programmers or public
 * @returns
 */
function filterChangelogs(changelogs, device, isClean) {
  const families = dataGetters.getFamilies()
  return changelogs.filter((changelog) => {
    if (isClean && changelog.notImportant) return false
    // no deivce specified do not filter by depends
    if (!device) return true
    let topic = dataGetters
      .getTopics(false)
      .find((topic) => changelog.topic === topic.name && changelog.category === topic.category)
    if (['Updates', 'CVE Patches'].includes(changelog.type)) topic = topic ?? {}
    const checkDepends = (depends) => {
      if (
        depends.some((depend) => depend.type === 'hw') &&
        !depends.every(
          (depend) => depend.type !== 'hw' || dataGetters.getDevicesHw()[device][depend.name] === (depend.value ?? true)
        )
      )
        return false
      if (depends.every((depend) => !['family', 'device'].includes(depend.type))) return true
      return depends.some((depend) => {
        if (depend.type === 'family') {
          const family = families.find((family) => family.name === depend.name)
          return family.devices.includes(device)
        }
        return depend.type === 'device' && depend.name === device
      })
    }
    if (topic.depends?.length > 0 && !checkDepends(topic.depends)) return false
    if (!changelog.depends || changelog.depends.length === 0) return true
    return checkDepends(changelog.depends)
  })
}

// Modifies changelog to have Improvements generated from Updates
function modifyChangelog(changelogs) {
  return changelogs.map((changelog) => {
    if (changelog.type === 'Updates')
      return {
        ...changelog,
        type: 'Improvements',
        change: `updated version to ${changelog.change}`,
        depends: changelog.depends ?? [],
        oldType: 'Updates',
      }
    else return changelog
  })
}

function groupBy(list, callbackFn) {
  const res = {}
  list.forEach((element) => {
    const key = callbackFn(element)
    if (res[key]) res[key].push(element)
    else res[key] = [element]
  })
  return res
}

/**
 * @param {[string, string]} a
 * @param {[string, string]} a
 * @returns {number}
 */
function sortByType(a, b) {
  const types = dataGetters.getTypes()
  return types.indexOf(a[0]) - types.indexOf(b[0])
}

/**
 * @param {[string, string]} a
 * @param {[string, string]} a
 * @returns {number}
 */
function sortByCategory(a, b) {
  const categories = dataGetters.getCategories()
  return categories.indexOf(a[0]) - categories.indexOf(b[0])
}
