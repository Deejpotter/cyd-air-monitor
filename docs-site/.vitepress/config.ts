import { defineConfig } from 'vitepress'

export default defineConfig({
  title: 'CYD Air Monitor',
  description: 'Board features, firmware capabilities, and flash guides for supported CYD displays',
  base: '/cyd-air-monitor/',
  head: [
    ['link', { rel: 'icon', href: '/cyd-air-monitor/favicon.svg', type: 'image/svg+xml' }],
  ],
  themeConfig: {
    logo: '/logo.svg',
    nav: [
      { text: 'Boards', link: '/boards/' },
      {
        text: 'By board',
        items: [
          { text: 'ESP32-2432S028R', link: '/boards/2432s028r' },
          { text: 'JC2432W328R', link: '/boards/jc2432w328r' },
          { text: 'JC2432W328C', link: '/boards/jc2432w328c' },
          { text: 'JC4827W543R', link: '/boards/jc4827w543r' },
        ],
      },
      { text: 'Flash guide', link: '/guide/getting-started' },
      { text: 'GitHub', link: 'https://github.com/Deejpotter/cyd-air-monitor' },
    ],
    sidebar: {
      '/boards/': [
        {
          text: 'Boards',
          items: [
            { text: 'Overview', link: '/boards/' },
            { text: 'ESP32-2432S028R', link: '/boards/2432s028r' },
            { text: 'JC2432W328R', link: '/boards/jc2432w328r' },
            { text: 'JC2432W328C', link: '/boards/jc2432w328c' },
            { text: 'JC4827W543R', link: '/boards/jc4827w543r' },
          ],
        },
        {
          text: 'Reference',
          items: [
            { text: 'Compare boards', link: '/boards/compare' },
            { text: 'Gotchas', link: '/boards/gotchas' },
            { text: 'External links', link: '/boards/references' },
          ],
        },
      ],
      '/guide/': [
        {
          text: 'Firmware',
          items: [
            { text: 'Flash & first run', link: '/guide/getting-started' },
            { text: 'Settings & features', link: '/guide/settings' },
            { text: 'Troubleshooting', link: '/guide/troubleshooting' },
          ],
        },
      ],
    },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/Deejpotter/cyd-air-monitor' },
    ],
    search: {
      provider: 'local',
    },
    footer: {
      message: 'One page per board — hardware features, firmware, pins, and flash commands',
      copyright: 'CYD Air Monitor firmware docs',
    },
  },
})
