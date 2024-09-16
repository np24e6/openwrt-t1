<template>
  <div>
    <div class="collapsable-card pb-6 border-light-tlt-4 border rounded mb-4 transition-all pt-6 px-4 md:mb-0 md:border-0 md:border-t md:rounded-none md:pt-8 md:pb-2 md:px-0">
      <div class="font-sans text-fish text-gray-tlt-100">
        <h3 class="mb-6">
          <div class="flex flex-wrap flex-row items-center gap-y-4 grow">
            <div
              class="title-info cursor-pointer flex items-center gap-1"
              @click="() => (responseExpanded = !responseExpanded)"
            >
              <button class="p-0.5">
                <svg
                  width="20"
                  height="20"
                  viewBox="0 0 20 20"
                  fill="none"
                  xmlns="http://www.w3.org/2000/svg"
                  aria-label="icon dropdown-arrow"
                  :class="{ 'rotate-180': responseExpanded }"
                >
                  <rect
                    width="20"
                    height="20"
                    fill="transparent"
                  ></rect>
                  <path
                    d="M5 8L9.87601 12.6544C10.0759 12.8452 10.3927 12.8376 10.5833 12.6375L15 8"
                    stroke="currentColor"
                    stroke-width="1.2"
                    stroke-linecap="round"
                  ></path>
                </svg></button
              ><span class="max-w-[min(100ch,60vw)] font-semibold truncate">Response</span>
            </div>
          </div>
        </h3>
      </div>
      <div
        v-show="responseExpanded"
        class="card-content"
      >
        <code>{{ response }}</code>
      </div>
    </div>
    <br />
    <div class="collapsable-card pb-6 border-light-tlt-4 border rounded mb-4 transition-all pt-6 px-4 md:mb-0 md:border-0 md:border-t md:rounded-none md:pt-8 md:pb-2 md:px-0">
      <div class="font-sans text-fish text-gray-tlt-100">
        <h3 class="mb-6">
          <div class="flex flex-wrap flex-row items-center gap-y-4 grow">
            <div
              class="title-info cursor-pointer flex items-center gap-1"
              @click="() => (functionsExampleExpanded = !functionsExampleExpanded)"
            >
              <div class="title-info cursor-pointer flex items-center gap-1">
                <button class="p-0.5">
                  <svg
                    width="20"
                    height="20"
                    viewBox="0 0 20 20"
                    fill="none"
                    xmlns="http://www.w3.org/2000/svg"
                    aria-label="icon dropdown-arrow"
                    :class="{ 'rotate-180': functionsExampleExpanded }"
                  >
                    <rect
                      width="20"
                      height="20"
                      fill="transparent"
                    ></rect>
                    <path
                      d="M5 8L9.87601 12.6544C10.0759 12.8452 10.3927 12.8376 10.5833 12.6375L15 8"
                      stroke="currentColor"
                      stroke-width="1.2"
                      stroke-linecap="round"
                    ></path>
                  </svg></button
                ><span class="max-w-[min(100ch,60vw)] font-semibold truncate">Functions Example</span>
              </div>
            </div>
          </div>
        </h3>
      </div>
      <div
        v-show="functionsExampleExpanded"
        class="card-content"
      >
        <div class="card-content pb-2 md:pb-14">
          <button
            class="disabled:bg-gray-tlt-600 w-max text-white bg-primary-300 hover:bg-primary-250 active:bg-primary-250 py-1.5 text-body-secondary px-4 font-semibold rounded aria-disabled:cursor-default transition-all disabled:text-gray-tlt-300 flex items-center font-sans shrink-0 h-max focus-visible:outline-2 focus-visible:outline-primary-300 outline-none"
            @click="functionExampleCall()"
          >
            {{ $t('Get API Example') }}
          </button>
        </div>
      </div>
    </div>
    <div class="collapsable-card pb-6 border-light-tlt-4 border rounded mb-4 transition-all pt-6 px-4 md:mb-0 md:border-0 md:border-t md:rounded-none md:pt-8 md:pb-2 md:px-0">
      <div class="font-sans text-fish text-gray-tlt-100">
        <h3 class="mb-6">
          <div class="flex flex-wrap flex-row items-center gap-y-4 grow">
            <div
              class="title-info cursor-pointer flex items-center gap-1"
              @click="() => (functionsExampleActionsExpanded = !functionsExampleActionsExpanded)"
            >
              <button class="p-0.5">
                <svg
                  width="20"
                  height="20"
                  viewBox="0 0 20 20"
                  fill="none"
                  xmlns="http://www.w3.org/2000/svg"
                  aria-label="icon dropdown-arrow"
                  :class="{ 'rotate-180': functionsExampleActionsExpanded }"
                >
                  <rect
                    width="20"
                    height="20"
                    fill="transparent"
                  ></rect>
                  <path
                    d="M5 8L9.87601 12.6544C10.0759 12.8452 10.3927 12.8376 10.5833 12.6375L15 8"
                    stroke="currentColor"
                    stroke-width="1.2"
                    stroke-linecap="round"
                  ></path>
                </svg></button
              ><span class="max-w-[min(100ch,60vw)] font-semibold truncate">Functions Example Actions</span>
            </div>
          </div>
        </h3>
      </div>
      <div
        v-show="functionsExampleActionsExpanded"
        class="card-content"
      >
        <div class="card-content pb-2 md:pb-14">
          <div class="items-start">
            <div
              class="text-body-secondary md:grid-cols-[40%_60%] mb-6 last:mb-0 only:mb-0 items-start grid"
              useautocomplete="false"
            >
              <div class="mr-4 flex gap-2 relative shrink-0 items-center md:justify-self-end mb:0 mb-2 w-full md:justify-end mt-1.5">
                <label
                  id="name-label"
                  class="text-gray-tlt-75 max-w-fit hover:text-primary-300 font-sans"
                  for="name"
                  >Name</label
                ><span class="font-semibold text-error-100 leading-3">*</span>
                <button class="h-5 w-5 md:hidden tooltip-button text-primary-gray-100 focus:text-primary-500">
                  <svg
                    width="20"
                    height="20"
                    viewBox="0 0 20 20"
                    fill="none"
                    xmlns="http://www.w3.org/2000/svg"
                    aria-label="icon tooltip"
                  >
                    <rect
                      x="2.5"
                      y="2.5"
                      width="15"
                      height="15"
                      rx="7.5"
                      stroke="currentColor"
                    ></rect>
                    <path
                      d="M10.061 11.5V11.29C10.061 10.61 10.481 10.25 10.901 9.96C11.311 9.68 11.721 9.32 11.721 8.66C11.721 7.74 10.981 7 10.061 7C9.14104 7 8.40104 7.74 8.40104 8.66M10.056 13.89H10.066"
                      stroke="currentColor"
                      stroke-width="1.5"
                      stroke-linecap="round"
                      stroke-linejoin="round"
                    ></path>
                  </svg>
                </button>
              </div>
              <div class="relative flex gap-2 w-full">
                <div class="tlt-input-wrapper">
                  <input
                    v-model="name"
                    type="text"
                    class="tlt-input-field input-size"
                    :data-state="nameError ? 'error' : ''"
                  />
                </div>
              </div>
            </div>
            <div class="text-body-secondary md:grid-cols-[40%_60%] mb-6 last:mb-0 only:mb-0 items-start grid">
              <div class="mr-4 flex gap-2 relative shrink-0 items-center md:justify-self-end mb:0 mb-2 w-full md:justify-end mt-1.5">
                <label
                  id="custom-label"
                  class="text-gray-tlt-75 max-w-fit hover:text-primary-300 font-sans"
                  for="custom"
                  >Custom</label
                ><button class="h-5 w-5 md:hidden tooltip-button text-primary-gray-100 focus:text-primary-500"></button>
              </div>
              <div class="relative flex gap-2 w-full">
                <div class="tlt-input-wrapper">
                  <input
                    v-model="custom"
                    type="text"
                    class="tlt-input-field input-size"
                    placeholder=""
                  />
                </div>
              </div>
            </div>
            <div class="text-body-secondary md:grid-cols-[40%_60%] mb-6 last:mb-0 only:mb-0 items-start grid">
              <div class="mr-4 flex gap-2 relative shrink-0 items-center md:justify-self-end mb:0 mb-2 w-full md:justify-end mt-1.5">
                <label
                  id="-label"
                  class="text-gray-tlt-75 max-w-fit hover:text-primary-300 font-sans"
                  for=""
                  >Execute Action</label
                >
              </div>
              <div class="relative flex gap-2 w-full">
                <button
                  class="disabled:bg-gray-tlt-600 gap-2 w-max text-white bg-primary-300 hover:bg-primary-250 active:bg-primary-250 py-1.5 text-body-secondary px-4 font-semibold rounded aria-disabled:cursor-default transition-all disabled:text-gray-tlt-300 flex items-center font-sans shrink-0 h-max focus-visible:outline-2 focus-visible:outline-primary-300 outline-none"
                  type="button"
                  @click="executeAction"
                >
                  POST API Example
                </button>
              </div>
            </div>

            <div class="text-body-secondary md:grid-cols-[40%_60%] mb-6 last:mb-0 only:mb-0 items-start grid">
              <div class="mr-4 flex gap-2 relative shrink-0 items-center md:justify-self-end mb:0 mb-2 w-full md:justify-end mt-1.5">
                <label
                  id="custom-label"
                  class="text-gray-tlt-75 max-w-fit hover:text-primary-300 font-sans"
                  for="custom"
                  >File Upload</label
                >
              </div>
              <div class="relative flex gap-2 w-full">
                <div class="relative flex gap-2 w-full">
                  <div
                    class="min-h-[32px] flex shrink-0 items-center max-w-[20rem] upload-wrapper"
                    dynamic-path=""
                  >
                    <div
                      id="upload"
                      class="flex items-center w-full"
                    >
                      <button
                        aria-disabled="false"
                        class="disabled:bg-transparent gap-2 w-max text-primary-300 hover:text-primary-250 text-body-secondary font-semibold rounded aria-disabled:cursor-default transition-all disabled:text-gray-tlt-300 flex items-center font-sans shrink-0 h-max focus-visible:outline-2 focus-visible:outline-primary-300 outline-none"
                        type="button"
                        @click="_fileSelect"
                      >
                        Browse
                      </button>
                      <!-- <span
                        class="dragText"
                        >or drag and drop your file here</span
                      > -->
                      <input
                        ref="inputRef"
                        type="file"
                        style="display: none"
                        @change="_uploadFile"
                      />
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      name: '',
      nameError: false,
      custom: '',
      response: '-',
      responseExpanded: true,
      functionsExampleExpanded: true,
      functionsExampleActionsExpanded: true
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
    executeAction() {
      this.$VuciValidator.value = this.name
      const { isValid } = this.$VuciValidator.defaulttype()
      if (!isValid) {
        this.nameError = true
        return this.$message.error(this.$t("'Name' is required field"))
      } else {
        this.nameError = false
        this.$axios
          .post('/api/example_f/actions/test', {
            data: {
              name: this.name,
              custom: this.custom
            }
          })
          .then(res => {
            this.response = JSON.stringify(res)
            this.$message.success(this.$t('Action executed'))
          })
          .catch(_ => {
            this.$message.error(this.$t('Unexpected error has occured'))
          })
      }
    },
    async _uploadFile(val) {
      const formData = new FormData()
      formData.append('option', 'file_upload')
      formData.append('file', val.target.files[0])
      try {
        const res = await this.$axios.post('/api/example_f/config', formData, {
          headers: {
            'Content-Type': 'multipart/form-data'
          },
          responseType: 'json'
        })
        if (res.success) {
          this.$message.success(this.$t('File uploaded successfully'))
        }
      } catch (error) {
        this.$message.error(this.$t('Failed to upload file'))
      }
    },
    _fileSelect() {
      this.$refs.inputRef.click()
    }
  }
}
</script>
