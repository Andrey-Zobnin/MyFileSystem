document.addEventListener('DOMContentLoaded', () => {
    // DOM Elements
    const fileList = document.getElementById('file-list');
    const currentPath = document.getElementById('current-path');
    const terminalContent = document.getElementById('terminal-content');
    const commandInput = document.getElementById('command-input');
    const newFolderBtn = document.getElementById('new-folder-btn');
    const newFileBtn = document.getElementById('new-file-btn');
    const uploadFileBtn = document.getElementById('upload-file-btn');
    const deleteBtn = document.getElementById('delete-btn');
    const modal = document.getElementById('modal');
    const modalTitle = document.getElementById('modal-title');
    const modalInput = document.getElementById('modal-input');
    const modalConfirm = document.getElementById('modal-confirm');
    const modalCancel = document.getElementById('modal-cancel');
    const closeModal = document.querySelector('.close');

    // State
    let selectedItem = null;
    let currentDirectory = '/';
    
    // Initialize
    loadDirectory(currentDirectory);
    addLog('Virtual File System initialized');
    
    // Event Listeners
    commandInput.addEventListener('keydown', (e) => {
        if (e.key === 'Enter') {
            const command = commandInput.value;
            addLog(`> ${command}`);
            executeCommand(command);
            commandInput.value = '';
        }
    });
    
    newFolderBtn.addEventListener('click', () => {
        showModal('Create Folder', '', 'folder');
    });
    
    newFileBtn.addEventListener('click', () => {
        showModal('Create File', '', 'file');
    });
    
    uploadFileBtn.addEventListener('click', () => {
        // Implement file upload functionality
        addLog('File upload functionality coming soon');
    });
    
    deleteBtn.addEventListener('click', () => {
        if (selectedItem) {
            const type = selectedItem.dataset.type;
            const name = selectedItem.dataset.name;
            const path = joinPath(currentDirectory, name);
            
            if (type === 'folder') {
                fetch(`/api/rmdir?path=${encodeURIComponent(path)}`)
                    .then(response => response.json())
                    .then(data => {
                        if (data.success) {
                            addLog(`Deleted directory: ${path}`);
                            loadDirectory(currentDirectory);
                        } else {
                            addLog(`Error: ${data.message}`, true);
                        }
                    })
                    .catch(error => {
                        addLog(`Error: ${error}`, true);
                    });
            } else {
                fetch(`/api/delete?path=${encodeURIComponent(path)}`)
                    .then(response => response.json())
                    .then(data => {
                        if (data.success) {
                            addLog(`Deleted file: ${path}`);
                            loadDirectory(currentDirectory);
                        } else {
                            addLog(`Error: ${data.message}`, true);
                        }
                    })
                    .catch(error => {
                        addLog(`Error: ${error}`, true);
                    });
            }
        }
    });
    
    modalConfirm.addEventListener('click', () => {
        const name = modalInput.value.trim();
        const action = modal.dataset.action;
        
        if (name) {
            const path = joinPath(currentDirectory, name);
            
            if (action === 'folder') {
                createFolder(path);
            } else if (action === 'file') {
                createFile(path);
            }
        }
        
        closeModalWindow();
    });
    
    modalCancel.addEventListener('click', closeModalWindow);
    closeModal.addEventListener('click', closeModalWindow);
    
    // Functions
    
    function loadDirectory(path) {
        fetch(`/api/ls?path=${encodeURIComponent(path)}`)
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    fileList.innerHTML = '';
                    currentPath.textContent = path;
                    
                    // Add parent directory if not at root
                    if (path !== '/') {
                        const parentItem = createListItem('..', 'folder', true);
                        fileList.appendChild(parentItem);
                    }
                    
                    // Add folders
                    data.items.forEach(item => {
                        const listItem = createListItem(item.name, item.type);
                        fileList.appendChild(listItem);
                    });
                    
                    // Reset selection
                    selectedItem = null;
                    deleteBtn.disabled = true;
                    currentDirectory = path;
                } else {
                    addLog(`Error: ${data.message}`, true);
                }
            })
            .catch(error => {
                addLog(`Error: ${error}`, true);
            });
    }
    
    function createListItem(name, type, isParent = false) {
        const item = document.createElement('div');
        item.className = 'file-item';
        item.dataset.name = name;
        item.dataset.type = type;
        
        const iconSpan = document.createElement('span');
        iconSpan.className = type === 'folder' ? 'file-icon folder-icon' : 'file-icon';
        iconSpan.textContent = type === 'folder' ? '📁' : '📄';
        
        const nameSpan = document.createElement('span');
        nameSpan.textContent = name;
        
        item.appendChild(iconSpan);
        item.appendChild(nameSpan);
        
        // Add event listeners
        item.addEventListener('click', (e) => {
            // Deselect previous item
            if (selectedItem) {
                selectedItem.classList.remove('selected');
            }
            
            if (!isParent) {
                // Select current item
                item.classList.add('selected');
                selectedItem = item;
                deleteBtn.disabled = false;
            } else {
                selectedItem = null;
                deleteBtn.disabled = true;
            }
        });
        
        item.addEventListener('dblclick', (e) => {
            if (type === 'folder') {
                // Navigate to folder
                let path;
                if (name === '..') {
                    // Go to parent directory
                    path = getParentPath(currentDirectory);
                } else {
                    path = joinPath(currentDirectory, name);
                }
                loadDirectory(path);
                addLog(`Changed directory to: ${path}`);
            } else {
                // Open file
                const path = joinPath(currentDirectory, name);
                addLog(`Opening file: ${path}`);
                // Implement file viewing functionality
                fetch(`/api/read?path=${encodeURIComponent(path)}`)
                    .then(response => response.json())
                    .then(data => {
                        if (data.success) {
                            showModal('File Content', data.content, 'view');
                        } else {
                            addLog(`Error: ${data.message}`, true);
                        }
                    })
                    .catch(error => {
                        addLog(`Error: ${error}`, true);
                    });
            }
        });
        
        return item;
    }
    
    function executeCommand(command) {
        if (!command) return;
        
        const parts = command.split(' ');
        const cmd = parts[0].toLowerCase();
        
        switch (cmd) {
            case 'cd':
                if (parts.length > 1) {
                    let path = parts[1];
                    if (!path.startsWith('/')) {
                        path = joinPath(currentDirectory, path);
                    }
                    loadDirectory(path);
                }
                break;
            case 'ls':
            case 'dir':
                loadDirectory(currentDirectory);
                break;
            case 'mkdir':
                if (parts.length > 1) {
                    let path = parts[1];
                    if (!path.startsWith('/')) {
                        path = joinPath(currentDirectory, path);
                    }
                    createFolder(path);
                }
                break;
            case 'rmdir':
                if (parts.length > 1) {
                    let path = parts[1];
                    if (!path.startsWith('/')) {
                        path = joinPath(currentDirectory, path);
                    }
                    fetch(`/api/rmdir?path=${encodeURIComponent(path)}`)
                        .then(response => response.json())
                        .then(data => {
                            if (data.success) {
                                addLog(`Deleted directory: ${path}`);
                                loadDirectory(currentDirectory);
                            } else {
                                addLog(`Error: ${data.message}`, true);
                            }
                        })
                        .catch(error => {
                            addLog(`Error: ${error}`, true);
                        });
                }
                break;
            case 'delete':
                if (parts.length > 1) {
                    let path = parts[1];
                    if (!path.startsWith('/')) {
                        path = joinPath(currentDirectory, path);
                    }
                    fetch(`/api/delete?path=${encodeURIComponent(path)}`)
                        .then(response => response.json())
                        .then(data => {
                            if (data.success) {
                                addLog(`Deleted file: ${path}`);
                                loadDirectory(currentDirectory);
                            } else {
                                addLog(`Error: ${data.message}`, true);
                            }
                        })
                        .catch(error => {
                            addLog(`Error: ${error}`, true);
                        });
                }
                break;
            case 'write':
                if (parts.length > 2) {
                    let path = parts[1];
                    let content = parts.slice(2).join(' ');
                    if (!path.startsWith('/')) {
                        path = joinPath(currentDirectory, path);
                    }
                    fetch('/api/write', {
                        method: 'POST',
                        headers: {
                            'Content-Type': 'application/json'
                        },
                        body: JSON.stringify({ path, content })
                    })
                        .then(response => response.json())
                        .then(data => {
                            if (data.success) {
                                addLog(`File written: ${path}`);
                                loadDirectory(currentDirectory);
                            } else {
                                addLog(`Error: ${data.message}`, true);
                            }
                        })
                        .catch(error => {
                            addLog(`Error: ${error}`, true);
                        });
                }
                break;
            case 'read':
                if (parts.length > 1) {
                    let path = parts[1];
                    if (!path.startsWith('/')) {
                        path = joinPath(currentDirectory, path);
                    }
                    fetch(`/api/read?path=${encodeURIComponent(path)}`)
                        .then(response => response.json())
                        .then(data => {
                            if (data.success) {
                                addLog(`File content: ${data.content}`);
                            } else {
                                addLog(`Error: ${data.message}`, true);
                            }
                        })
                        .catch(error => {
                            addLog(`Error: ${error}`, true);
                        });
                }
                break;
            case 'pwd':
                addLog(`Current directory: ${currentDirectory}`);
                break;
            case 'help':
                addLog(`
Available commands:
cd <path>          - Change directory
ls, dir            - List files and directories
mkdir <path>       - Create directory
rmdir <path>       - Remove directory
write <path> <txt> - Write text to a file
read <path>        - Read file content
delete <path>      - Delete file
pwd                - Print current directory
help               - Show this help
                `);
                break;
            default:
                addLog(`Unknown command: ${cmd}`, true);
                break;
        }
    }
    
    function createFolder(path) {
        fetch(`/api/mkdir?path=${encodeURIComponent(path)}`)
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    addLog(`Created directory: ${path}`);
                    loadDirectory(currentDirectory);
                } else {
                    addLog(`Error: ${data.message}`, true);
                }
            })
            .catch(error => {
                addLog(`Error: ${error}`, true);
            });
    }
    
    function createFile(path) {
        fetch('/api/write', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ path, content: '' })
        })
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    addLog(`Created file: ${path}`);
                    loadDirectory(currentDirectory);
                } else {
                    addLog(`Error: ${data.message}`, true);
                }
            })
            .catch(error => {
                addLog(`Error: ${error}`, true);
            });
    }
    
    function addLog(message, isError = false) {
        const logLine = document.createElement('div');
        logLine.textContent = message;
        if (isError) {
            logLine.className = 'error';
        }
        terminalContent.appendChild(logLine);
        terminalContent.scrollTop = terminalContent.scrollHeight;
    }
    
    function showModal(title, content, action) {
        modalTitle.textContent = title;
        modalInput.value = content;
        modal.dataset.action = action;
        
        if (action === 'view') {
            modalInput.readOnly = true;
        } else {
            modalInput.readOnly = false;
        }
        
        modal.style.display = 'block';
        modalInput.focus();
    }
    
    function closeModalWindow() {
        modal.style.display = 'none';
        modalInput.value = '';
    }
    
    function joinPath(dir, file) {
        if (dir.endsWith('/')) {
            return dir + file;
        } else {
            return dir + '/' + file;
        }
    }
    
    function getParentPath(path) {
        if (path === '/') return '/';
        
        // Remove trailing slash if exists
        if (path.endsWith('/')) {
            path = path.slice(0, -1);
        }
        
        const lastSlashIndex = path.lastIndexOf('/');
        if (lastSlashIndex === 0) {
            return '/';
        } else {
            return path.substring(0, lastSlashIndex);
        }
    }
});