document.addEventListener("DOMContentLoaded", () => {
    const fileList = document.getElementById("file-list");
    const uploadForm = document.getElementById("upload-form");
    const fileInput = document.getElementById("file");

    const fetchFiles = async () => {
        try {
            const response = await fetch("/cgi/file_handler.cgi");
            const result = await response.json();
            if (result.success) {
                fileList.innerHTML = result.message.map(file => `
                    <tr>
                        <td>${file.name}</td>
                        <td>${file.size}</td>
                        <td><button onclick="deleteFile('${file.name}')">Delete</button></td>
                    </tr>
                `).join('');
            } else {
                console.error("Error fetching files:", result.error);
            }
        } catch (error) {
            console.error("Error fetching files:", error);
        }
    };

    window.deleteFile = async (fileName) => {
        try {
            const response = await fetch(`/cgi/file_handler.cgi/${encodeURIComponent(fileName)}`, { method: "DELETE" });
            const result = await response.json();
            result.success ? fetchFiles() : console.error("Error deleting file:", result.error);
        } catch (error) {
            console.error("Error deleting file:", error);
        }
    };

    const uploadFile = async (file) => {
        const formData = new FormData();
        formData.append("file", file);
        try {
            const response = await fetch("/cgi/file_handler.cgi", { method: "POST", body: formData });
            const result = await response.json();
            result.success ? fetchFiles() : console.error("Error uploading file:", result.error);
        } catch (error) {
            console.error("Error uploading file:", error);
        }
    };

    uploadForm.addEventListener("submit", (event) => {
        event.preventDefault();
        const file = fileInput.files[0];
        if (file) uploadFile(file);
    });

    fetchFiles();
});