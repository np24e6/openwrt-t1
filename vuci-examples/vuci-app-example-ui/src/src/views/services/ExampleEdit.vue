<template>
    <vuci-form-api
      v-slot="{ uciData }"
      config="example"
      editing
    >
      <vuci-named-section-api
        v-slot="{ s }"
        :title="$t('Example configuration: %s').format(section.id)"
        :uci-data="uciData"
        :endpoints="[{ endpoint: 'example_c/config' }]"
        :name="section.id"
        data-key="example"
      >
        <vuci-form-item-switch-api
          :uci-section="s"
          :label="$t('Bool')"
          name="bool"
        />
        <vuci-form-item-input-api
          :uci-section="s"
          :label="$t('Text')"
          name="text"
          maxlength="100"
          minlength="5"
        />
        <vuci-form-item-select-api
          :uci-section="s"
          :label="$t('Select')"
          name="select"
          :options="selectOptions"
        />
        <vuci-form-item-select-api
          :uci-section="s"
          name="multi_select"
          :label="$t('Select Multiple')"
          :placeholder="$t('Any')"
          :options="selectOptions"
          maxlength="64"
          multiple
          allow-create
        />
      </vuci-named-section-api>
    </vuci-form-api>
  </template>
  
  <script>
  export default {
    props: {
      section: {
        type: Object,
        required: true
      }
    },
    data() {
      return {
        selectOptions: [
          ['first', this.$t('Example 1')],
          ['second', this.$t('Example 2')]
        ]
      }
    }
  }
  </script>
  