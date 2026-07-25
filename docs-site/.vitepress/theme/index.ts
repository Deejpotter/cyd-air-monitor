import DefaultTheme from 'vitepress/theme'
import './custom.css'
import Layout from './Layout.vue'
import BoardCardGrid from '../components/BoardCardGrid.vue'
import BoardPage from '../components/BoardPage.vue'
import BoardExplorer from '../components/BoardExplorer.vue'
import BoardGotchas from '../components/BoardGotchas.vue'
import BoardGotchaSummary from '../components/BoardGotchaSummary.vue'
import BoardReferences from '../components/BoardReferences.vue'
import BoardMatrix from '../components/BoardMatrix.vue'
import ReferencesList from '../components/ReferencesList.vue'
import TroubleshootingGuide from '../components/TroubleshootingGuide.vue'

export default {
  extends: DefaultTheme,
  Layout,
  enhanceApp({ app }) {
    app.component('BoardCardGrid', BoardCardGrid)
    app.component('BoardPage', BoardPage)
    app.component('BoardExplorer', BoardExplorer)
    app.component('BoardGotchas', BoardGotchas)
    app.component('BoardGotchaSummary', BoardGotchaSummary)
    app.component('BoardReferences', BoardReferences)
    app.component('BoardMatrix', BoardMatrix)
    app.component('ReferencesList', ReferencesList)
    app.component('TroubleshootingGuide', TroubleshootingGuide)
  },
}
