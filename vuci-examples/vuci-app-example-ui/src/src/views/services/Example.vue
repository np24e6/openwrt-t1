<template>
  <div>
    <tlt-card :title="$t('Response')">
      <code>{{ response }}</code>
    </tlt-card>
    <tlt-card :title="$t('Functions Example')">
      <tlt-button @click="functionExampleCall()">
        {{ $t('Get API Example') }}
      </tlt-button>
    </tlt-card>
    <tlt-form
      ref="fExampleForm"
      :model="form"
      custom-save
      sid="example"
      :title="$t('Functions Example Actions')"
    >
      <tlt-form-item-input
        v-model="form.name"
        :help="$t('Example of actions with input name')"
        :label="$t('Name')"
        prop="name"
        maxlength="256"
        required
      />
      <tlt-form-item-input
        v-model="form.custom"
        :help="$t('Example of actions with input custom')"
        :label="$t('Custom')"
        prop="custom"
        :rules="validateCustom"
      />
      <tlt-form-model-item
        :label="$t('Execute Action')"
        element-id="action"
      >
        <tlt-button @click="executeAction">
          {{ $t('POST API Example') }}
        </tlt-button>
      </tlt-form-model-item>
      <tlt-form-model-item :label="$t('File Upload')">
        <tlt-upload-api
          instant
          name="file_upload"
          dynamic-path
          action="/api/example_f/config"
          path="/tmp/"
          @uploaded="handleAfterUpload"
        />
      </tlt-form-model-item>
    </tlt-form>
    <vuci-form-api
      v-slot="{ uciData }"
      v-model="cExampleForm"
      config="example"
      async-load
    >
      <vuci-typed-section-api
        type="example"
        data-key="example"
        :title="$t('Config Example')"
        :columns="configColumns"
        :edit-form="editModal"
        :endpoints="[{ endpoint: 'example_c/config' }]"
        :uci-data="uciData"
      >
        <template #id="{ s }">
          <vuci-form-item-dummy-api
            :uci-section="s"
            name="id"
          />
        </template>
        <template #test="{ s }">
          <vuci-form-item-dummy-api
            :uci-section="s"
            name="test"
          />
        </template>
        <template #bool="{ s }">
          <vuci-form-item-switch-api
            :uci-section="s"
            name="bool"
          />
        </template>
        <template #addForm="{ addModel }">
          <tlt-form-item-input
            v-model="addModel.id"
            :label="$t('Name')"
            prop="id"
            required
          />
        </template>
      </vuci-typed-section-api>
    </vuci-form-api>
  </div>
</template>

<script>
import EditForm from './ExampleEdit.vue'
export default {
  data() {
    return {
      response: '-',
      form: {
        name: '',
        custom: ''
      },
      cExampleForm: {},
      editModal: EditForm,
      configColumns: [
        {
          name: 'id',
          label: this.$t('Name')
        },
        {
          name: 'test',
          label: this.$t('Test')
        },
        {
          name: 'bool',
          label: this.$t('Bool')
        }
      ]
    }
  },
  methods: {
    functionExampleCall() {
      this.$spin(true)
      return this.$axios
        .get('/api/example_f/test')
        .then(res => {
          this.response = JSON.stringify(res)
        })
        .catch(() => {
          this.$message.error(this.$t('Failed to get example'))
        })
        .finally(() => {
          this.$spin(false)
        })
    },
    validateCustom(value) {
      if (value === 'value error') {
        return { isValid: false, message: this.$t('Example error message') }
      }
      return { isValid: true }
    },
    executeAction() {
      this.$refs.fExampleForm.validate().then(validationResult => {
        if (!validationResult.valid) return this.$message.error(this.$t('Some fields are invalid'))
        const data = this.$refs.fExampleForm.getData()
        return new Promise(resolve => {
          this.$axios
            .post('/api/example_f/actions/test', {
              data: {
                name: data.name,
                custom: data.custom
              }
            })
            .then(res => {
              this.response = JSON.stringify(res)
              this.$message.success(this.$t('Action executed'))
              resolve()
            })
            .catch(_ => {
              this.$message.error(this.$t('Unexpected error has occured'))
            })
        })
      })
    },
    async handleAfterUpload(e) {
      this.response = JSON.stringify(e.res)
      this.$spin(this.$t('Verifying file...'))
      if (e.res.success) {
        this.$message.success(this.$t('File uploaded successfully'))
      }
      this.$spin(false)
    }
  }
}
</script>
