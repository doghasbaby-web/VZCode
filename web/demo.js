// VZCode Demo Application
class VZCodeDemo {
    constructor() {
        this.vzcode = null;
        this.chart = null;
        this.currentData = null;
        this.isPlaying = false;
        this.currentStep = 0;

        this.initializeWASM();
        this.setupEventListeners();
        this.loadApiKeyFromStorage();
    }

    async initializeWASM() {
        try {
            // Wait for WASM module to load
            if (typeof VZCodeModule !== 'undefined') {
                const module = await VZCodeModule();
                this.vzcode = new module.VZCodeAPI();
                this.showStatus('WASM module loaded successfully', 'success');
            } else {
                this.showStatus('WASM module not available. Some features may be limited.', 'info');
            }
        } catch (error) {
            console.error('Failed to initialize WASM:', error);
            this.showStatus('Failed to load WASM module: ' + error.message, 'error');
        }
    }

    setupEventListeners() {
        document.getElementById('save-key-btn').addEventListener('click', () => this.saveApiKey());
        document.getElementById('analyze-btn').addEventListener('click', () => this.analyzeCode());
        document.getElementById('clear-btn').addEventListener('click', () => this.clearInput());
        document.getElementById('copy-mermaid-btn')?.addEventListener('click', () => this.copyMermaid());
        document.getElementById('play-btn')?.addEventListener('click', () => this.playAnimation());
        document.getElementById('pause-btn')?.addEventListener('click', () => this.pauseAnimation());
        document.getElementById('reset-btn')?.addEventListener('click', () => this.resetAnimation());
    }

    loadApiKeyFromStorage() {
        const savedKey = localStorage.getItem('gemini_api_key');
        if (savedKey) {
            document.getElementById('api-key').value = savedKey;
            if (this.vzcode) {
                this.vzcode.setApiKey(savedKey);
            }
        }
    }

    saveApiKey() {
        const apiKey = document.getElementById('api-key').value.trim();
        if (!apiKey) {
            this.showStatus('Please enter an API key', 'error');
            return;
        }

        localStorage.setItem('gemini_api_key', apiKey);

        if (this.vzcode) {
            this.vzcode.setApiKey(apiKey);
            this.showStatus('API key saved successfully', 'success');
        } else {
            this.showStatus('WASM module not loaded yet', 'error');
        }
    }

    async analyzeCode() {
        const code = document.getElementById('code-input').value.trim();
        if (!code) {
            this.showStatus('Please enter some code to analyze', 'error');
            return;
        }

        const apiKey = document.getElementById('api-key').value.trim();
        if (!apiKey) {
            this.showStatus('Please enter and save your API key first', 'error');
            return;
        }

        const language = document.getElementById('language').value;
        const vizType = document.getElementById('viz-type').value;

        this.showProgress(true);
        this.showStatus('Analyzing code with Gemini AI...', 'info');

        try {
            if (this.vzcode) {
                // Use WASM version
                const resultJson = this.vzcode.generateVisualization(code, language, vizType);
                const result = JSON.parse(resultJson);

                if (result.success) {
                    await this.displayResults(result);
                    this.showStatus('Analysis completed successfully!', 'success');
                } else {
                    this.showStatus('Error: ' + result.error, 'error');
                }
            } else {
                // Fallback: Direct API call (for demo purposes)
                await this.analyzeWithDirectAPI(code, language, vizType, apiKey);
            }
        } catch (error) {
            console.error('Analysis error:', error);
            this.showStatus('Error during analysis: ' + error.message, 'error');
        } finally {
            this.showProgress(false);
        }
    }

    async analyzeWithDirectAPI(code, language, vizType, apiKey) {
        // This is a fallback method that makes direct API calls
        // In production, this would be handled by the WASM module

        const prompt = `Analyze the following ${language} code and generate a ${vizType} diagram using Mermaid syntax.

Requirements:
1. Generate ONLY the Mermaid diagram code, wrapped in \`\`\`mermaid and \`\`\` tags
2. Keep the diagram clear and focused on the most important elements
3. Use descriptive labels for nodes and edges

Code to analyze:

\`\`\`${language}
${code}
\`\`\`

Please provide the ${vizType} diagram in Mermaid format.`;

        const response = await fetch(`https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent?key=${apiKey}`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                contents: [{
                    parts: [{
                        text: prompt
                    }]
                }]
            })
        });

        const data = await response.json();

        if (data.candidates && data.candidates[0]) {
            const text = data.candidates[0].content.parts[0].text;
            const mermaidMatch = text.match(/```mermaid\n([\s\S]*?)\n```/);
            const mermaid = mermaidMatch ? mermaidMatch[1] : text;

            await this.displayResults({
                success: true,
                mermaid: mermaid,
                description: 'Analysis generated successfully',
                vizzuData: this.convertMermaidToVizzu(mermaid)
            });
        } else {
            throw new Error('No response from Gemini API');
        }
    }

    convertMermaidToVizzu(mermaid) {
        // Simple conversion for demo
        return {
            series: [
                {
                    name: 'Component',
                    values: ['Start', 'Process', 'End']
                },
                {
                    name: 'Value',
                    values: [1, 2, 3]
                }
            ]
        };
    }

    async displayResults(result) {
        // Show results section
        document.querySelector('.results-section').style.display = 'block';

        // Display description
        document.getElementById('description').textContent = result.description || 'Analysis completed';

        // Display Mermaid diagram
        document.getElementById('mermaid-output').textContent = result.mermaid;

        // Create Vizzu visualization
        if (result.vizzuData) {
            await this.createVisualization(result.vizzuData);
        }
    }

    async createVisualization(vizzuData) {
        try {
            const container = document.getElementById('vizzu-container');
            container.innerHTML = ''; // Clear previous visualization

            this.chart = new Vizzu('vizzu-container', { data: vizzuData });

            // Initial animation
            await this.chart.animate({
                config: {
                    x: vizzuData.series[0]?.name || 'Component',
                    y: vizzuData.series[1]?.name || 'Value',
                    color: vizzuData.series[0]?.name || 'Component'
                }
            });

            this.currentData = vizzuData;
            this.updateStepInfo();
        } catch (error) {
            console.error('Visualization error:', error);
            this.showStatus('Error creating visualization: ' + error.message, 'error');
        }
    }

    async playAnimation() {
        if (!this.chart || this.isPlaying) return;

        this.isPlaying = true;

        // Example animation sequence
        await this.chart.animate({
            config: {
                coordSystem: 'polar'
            }
        }, {
            duration: 1000
        });

        await this.chart.animate({
            config: {
                coordSystem: 'cartesian'
            }
        }, {
            duration: 1000
        });

        this.isPlaying = false;
    }

    pauseAnimation() {
        this.isPlaying = false;
    }

    resetAnimation() {
        this.isPlaying = false;
        this.currentStep = 0;
        if (this.currentData) {
            this.createVisualization(this.currentData);
        }
    }

    updateStepInfo() {
        const stepInfo = document.getElementById('step-info');
        if (stepInfo) {
            stepInfo.textContent = `Step ${this.currentStep + 1}`;
        }
    }

    clearInput() {
        document.getElementById('code-input').value = '';
        document.querySelector('.results-section').style.display = 'none';
    }

    copyMermaid() {
        const mermaidText = document.getElementById('mermaid-output').textContent;
        navigator.clipboard.writeText(mermaidText).then(() => {
            this.showStatus('Mermaid diagram copied to clipboard!', 'success');
        });
    }

    showStatus(message, type) {
        const statusEl = document.getElementById('status');
        statusEl.textContent = message;
        statusEl.className = `status-message ${type}`;
        statusEl.style.display = 'block';

        setTimeout(() => {
            statusEl.style.display = 'none';
        }, 5000);
    }

    showProgress(show) {
        const progressEl = document.getElementById('progress');
        progressEl.style.display = show ? 'block' : 'none';
    }
}

// Initialize demo when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    window.vzcodeDemo = new VZCodeDemo();
});
